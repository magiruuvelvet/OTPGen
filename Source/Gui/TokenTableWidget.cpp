#include "TokenTableWidget.hpp"

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

                if (!isColumnHidden(3))
                {
                    if (qobject_cast<QCheckBox*>(cellWidget(i, 0)))
                    {
                        const auto checked = tokenShow(i)->isChecked();
                        tokenSecretWidget(i)->setVisible(checked);
                        tokenSecret(i)->setVisible(checked);
                        tokenSecretTimeout(i)->setVisible(checked);
                    }
                }
            }

            for (auto&& row : invalidRows)
            {
                tokenShow(row)->setChecked(true);
                tokenSecret(row)->setText("INVALID");
                tokenSecretTimeout(row)->setVisible(false);
                tokenControlWidget(row)->setDisabled(true);
                tokenSecretWidget(row)->setDisabled(true);
            }
        }
    }

    else if (mode == Mode::Export)
    {
        for (auto i = 0; i < rowCount(); i++)
        {
            tokenEditType(i)->setDisabled(true);
            tokenEditLabel(i)->setReadOnly(true);
            tokenEditSecret(i)->setDisabled(true);
            if (tokenEditSecretComboBoxExtra(i))
            {
                tokenEditSecretComboBoxExtra(i)->setDisabled(true);
            }
            tokenDigits(i)->setDisabled(true);
            tokenPeriod(i)->setDisabled(true);
            tokenCounter(i)->setDisabled(true);
            cellWidget(i, 6)->setDisabled(true);
        }
    }
}
