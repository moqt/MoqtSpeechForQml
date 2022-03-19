#include <QtQml/QQmlEngineExtensionPlugin>
#include <qdebug.h>

//![plugin]
class MoqtSpeech : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)
};
//![plugin]

#include "plugin.moc"
