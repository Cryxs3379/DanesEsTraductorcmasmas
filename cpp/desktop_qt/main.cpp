#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
#include <QDir>
#include "MainWindow.h"
#include "../core/TranslatorEngine.h"
#include "../core/Config.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Force cwd to executable directory (so ./models/... always resolves correctly)
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    
    // Set application properties
    QApplication::setApplicationName("Traductor Danés-Español");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("TraductorApp");
    
    // Initialize translator engine with explicit model paths
    traductor::Config config;
    
    // Set explicit model paths for GUI (relative to executable location)
    config.setModelDir("./models/nllb-600m");
    config.setCt2Dir("./models/nllb-600m-ct2-int8");
    
    traductor::TranslatorEngine translator(config);
    
    // Show loading message
    QMessageBox::information(nullptr, "Inicializando", 
        "Cargando modelo de traducción...\n"
        "Esto puede tomar unos segundos la primera vez.");
    
    // Initialize in background (in real implementation, you'd use threads)
    if (!translator.initialize()) {
        auto health = translator.getHealthInfo();
        QMessageBox::critical(nullptr, "Error de Inicialización",
            QString("No se pudo inicializar el traductor.\n"
                   "Error: %1").arg(QString::fromStdString(health.lastError)));
        return 1;
    }
    
    // Create and show main window
    MainWindow window(translator);
    window.show();
    
    return app.exec();
}
