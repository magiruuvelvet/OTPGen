#ifdef OTPGEN_GUI
#include "SvgTool.hpp"

#include <QDomDocument>

static void SetAttrRecur(QDomElement elem, QString strtagname, QString strattr, QString strattrval)
{
    // if it has the tagname then overwritte desired attribute
    if (elem.tagName().compare(strtagname) == 0)
    {
        elem.setAttribute(strattr, strattrval);
    }

    // loop all children
    for (auto i = 0; i < elem.childNodes().count(); i++)
    {
        if (!elem.childNodes().at(i).isElement())
        {
            continue;
        }
        SetAttrRecur(elem.childNodes().at(i).toElement(), strtagname, strattr, strattrval);
    }
}

void SvgTool::changeFillColor(std::string &svg, const std::string &color)
{
    QDomDocument doc;
    doc.setContent(QByteArray(svg.data()));

    const auto fill = QString::fromUtf8(color.c_str());

    // TODO: add all fillable SVG tags
    SetAttrRecur(doc.documentElement(), "path", "fill", fill);
    SetAttrRecur(doc.documentElement(), "polygon", "fill", fill);
    SetAttrRecur(doc.documentElement(), "circle", "fill", fill);
    auto str = doc.toByteArray();
    svg = std::string(str.constData(), str.constData() + str.size());
}

#endif
