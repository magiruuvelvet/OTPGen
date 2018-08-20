#include "TokenTableWidget.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QProgressBar>

TokenTableWidget::TokenTableWidget(Mode mode, QWidget *parent)
    : QTableWidget(parent), mode(mode), updating(false)
{
}

void TokenTableWidget::setUpdating(bool updating)
{
    this->updating = updating;
}

void TokenTableWidget::setInvalidRows(const QList<int> &rows)
{
    invalidRows = rows;
}

void TokenTableWidget::paintEvent(QPaintEvent *event)
{
    QTableWidget::paintEvent(event);

    // workaround an issue which doesn't hide cell widgets in the constructor

    if (mode == Mode::View)
    {
        if (!updating)
        {
            for (auto i = 0; i < rowCount(); i++)
            {
                // required for MainWindow::filterTokens() to function correctly
                // don't make the "INVALID" fields visible again if they are hidden by
                // setRowHidden()
                if (visualItemRect(itemAt(i, 0)).isEmpty())
                    continue;

                if (qobject_cast<QCheckBox*>(cellWidget(i, 0)))
                {
                    const auto checked = cellWidget(i, 0)->findChild<QCheckBox*>()->isChecked();
                    cellWidget(i, 3)->setVisible(checked);
                    cellWidget(i, 3)->findChild<QLineEdit*>()->setVisible(checked);
                    cellWidget(i, 3)->findChild<QProgressBar*>()->setVisible(checked);
                }
            }

            for (auto&& row : invalidRows)
            {
                cellWidget(row, 0)->findChild<QCheckBox*>()->setChecked(true);
                cellWidget(row, 3)->findChild<QLineEdit*>()->setText("INVALID");
                cellWidget(row, 3)->findChild<QProgressBar*>()->setVisible(false);
                cellWidget(row, 0)->setDisabled(true);
                cellWidget(row, 3)->setDisabled(true);
            }
        }
    }
}
