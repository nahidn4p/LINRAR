#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QPalette>
#include <QColor>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("LINRAR");
    app.setApplicationDisplayName("LINRAR - Linux Archive Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LINRAR");
    
    // Try to set a modern style
    QStringList styles = QStyleFactory::keys();
    if (styles.contains("Fusion")) {
        app.setStyle("Fusion");
    }
    
    // Set a modern color palette
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(35, 35, 35));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(palette);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
