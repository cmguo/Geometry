#ifndef GEOMETRYTOOL_H
#define GEOMETRYTOOL_H

#include <tools/menutool.h>

class GeometryTools : QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE GeometryTools();

public slots:
    static void getToolButtons(QList<ToolButton*> & result, QByteArray const & type);

    static void handleToolButton(ToolButton* button, QByteArray const & type);
};

class GeometryTool : public MenuTool
{
    Q_OBJECT
public:
    Q_INVOKABLE GeometryTool(ResourceView *res = nullptr);

private:
    virtual void getToolButtons(QList<ToolButton *> & buttons,
                                ToolButton * parent) override;

    using ToolButtonProvider::handleToolButton;

    virtual bool handleToolButton(ToolButton * button, QStringList const & args) override;

private:
    QList<ToolButton *> buttons_;
};

#endif // GRAPHTOOL_H
