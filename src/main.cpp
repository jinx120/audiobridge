#include <QtWidgets/QApplication>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "../include/mainwindow.h"

/**
 * @brief Loads and applies a style sheet from a file.
 * @param app The application instance.
 * @param path The path to the style sheet file.
 * @return True if the style sheet was loaded and applied successfully, false otherwise.
 */
bool loadStyleSheet(QApplication &app, const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return false;
    }
    
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());
    file.close();
    
    return true;
}

/**
 * @brief Application entry point.
 * @param argc Command line argument count.
 * @param argv Command line arguments.
 * @return Application exit code.
 */
int main(int argc, char *argv[])
{
    // Create the application
    QApplication app(argc, argv);
    
    // Set application information
    app.setApplicationName("AudioBridge");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("AudioBridge");
    app.setOrganizationDomain("audiobridge.example.com");
    
    // Load the default light style sheet
    loadStyleSheet(app, ":/styles/light_style.qss");
    
    // Create and show the main window
    MainWindow mainWindow;
    mainWindow.show();
    
    // Enter the application event loop
    return app.exec();
}
