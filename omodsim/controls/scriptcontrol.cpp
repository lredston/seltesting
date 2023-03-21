#include <QLoggingCategory>
#include "modbusmultiserver.h"
#include "scriptcontrol.h"
#include "ui_scriptcontrol.h"

///
/// \brief ScriptControl::ScriptControl
/// \param parent
///
ScriptControl::ScriptControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScriptControl)
    ,_script(new Script)
    ,_storage(new Storage)
    ,_server(nullptr)
    ,_console(nullptr)
{
    ui->setupUi(this);
    ui->codeEditor->moveCursor(QTextCursor::End);

    _console = QSharedPointer<console>(new console(ui->console));

    _jsEngine.globalObject().setProperty("console", _jsEngine.newQObject(_console.get()));
    _jsEngine.globalObject().setProperty("Script", _jsEngine.newQObject(_script.get()));
    _jsEngine.globalObject().setProperty("Storage", _jsEngine.newQObject(_storage.get()));

    connect(&_timer, &QTimer::timeout, this, &ScriptControl::executeScript);
    connect(_script.get(), &Script::stopped, this, &ScriptControl::stopScript);
}

///
/// \brief ScriptControl::~ScriptControl
///
ScriptControl::~ScriptControl()
{
    delete ui;
}

///
/// \brief ScriptControl::initJSEngine
/// \param server
/// \param order
///
void ScriptControl::initJSEngine(ModbusMultiServer& server, const ByteOrder& order)
{
    _server = QSharedPointer<Server>(new Server(server, order));
    _jsEngine.globalObject().setProperty("Server", _jsEngine.newQObject(_server.get()));
}

///
/// \brief ScriptControl::isAutoCompleteEnabled
/// \return
///
bool ScriptControl::isAutoCompleteEnabled() const
{
    return ui->codeEditor->isAutoCompleteEnabled();
}

///
/// \brief ScriptControl::enableAutoComplete
/// \param enable
///
void ScriptControl::enableAutoComplete(bool enable)
{
    ui->codeEditor->enableAutoComplete(enable);
}

///
/// \brief ScriptControl::script
/// \return
///
QString ScriptControl::script() const
{
    return ui->codeEditor->toPlainText();
}

///
/// \brief ScriptControl::setScript
/// \param text
///
void ScriptControl::setScript(const QString& text)
{
    ui->codeEditor->setPlainText(text);
    ui->codeEditor->moveCursor(QTextCursor::End);
}

///
/// \brief ScriptControl::undo
///
void ScriptControl::undo()
{
    ui->codeEditor->undo();
}

///
/// \brief ScriptControl::redo
///
void ScriptControl::redo()
{
    ui->codeEditor->redo();
}

///
/// \brief ScriptControl::cut
///
void ScriptControl::cut()
{
    ui->codeEditor->cut();
}

///
/// \brief ScriptControl::copy
///
void ScriptControl::copy()
{
    ui->codeEditor->copy();
}

///
/// \brief ScriptControl::paste
///
void ScriptControl::paste()
{
    ui->codeEditor->paste();
}

///
/// \brief ScriptControl::search
/// \param text
///
void ScriptControl::search(const QString& text)
{

}

///
/// \brief ScriptControl::isRunning
///
bool ScriptControl::isRunning() const
{
   return _timer.isActive();
}

///
/// \brief ScriptControl::setFocus
///
void ScriptControl::setFocus()
{
    ui->codeEditor->setFocus();
}

///
/// \brief ScriptControl::runScript
/// \param interval
///
void ScriptControl::runScript(RunMode mode, int interval)
{
    _console->clear();
    _scriptCode = script();
    _jsEngine.setInterrupted(false);

    if(!executeScript())
        return;

    switch(mode)
    {
        case RunMode::Once:
        break;

        case RunMode::Periodically:
            _timer.start(qMax(1000, interval));
        break;
    }
}

///
/// \brief ScriptControl::stopScript
///
void ScriptControl::stopScript()
{
    _timer.stop();
    _storage->clear();
    _jsEngine.setInterrupted(true);
}

///
/// \brief ScriptControl::executeScript
///
bool ScriptControl::executeScript()
{
    const auto res = _jsEngine.evaluate(_scriptCode);
    if(res.isError() && !_jsEngine.isInterrupted())
    {
        _console->error(QString("%1 (line %2)").arg(res.toString(), res.property("lineNumber").toString()));
        stopScript();

        return false;
    }
    return true;
 }
