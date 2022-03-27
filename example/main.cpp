#include <QGuiApplication>
#include <QQmlApplicationEngine>


// FIXME how to avoid the direct call of registration function?
#define REGISTER_STATIC_QML_PLUGIN(name) { \
    extern void qml_register_types_##name(); \
    qml_register_types_##name(); }


int main(int argc, char *argv[])
{
    REGISTER_STATIC_QML_PLUGIN(org_moqt_speech)

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url("qrc:/main.qml");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
