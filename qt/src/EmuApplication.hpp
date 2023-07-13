#pragma once
#include <QApplication>
#include <QTimer>
#include <QThread>

#include "EmuMainWindow.hpp"
#include "EmuConfig.hpp"
#include "SDLInputManager.hpp"
#include "Snes9xController.hpp"
#include "common/audio/s9x_sound_driver.hpp"

struct EmuThread : public QThread
{
Q_OBJECT
  public:
    EmuThread(QThread *main_thread);
    QThread *main_thread;
    void run() override;
    void waitForStatusBit(int);
    void waitForStatusBitCleared(int);
    void setStatusBits(int);
    void unsetStatusBits(int);
    void pause();
    void unpause();
    void setMainLoop(std::function<void()> loop);

    std::function<void()> main_loop = nullptr;

    enum StatusBits
    {
        eDead       = 0,
        ePaused     = 1,
        eSuspended  = 2,
        eRunning    = 4,
        eQuit       = 8
    };

    int status = eDead;
    std::mutex status_mutex;
    std::condition_variable status_cond;

  public slots:
    void runOnThread(std::function<void()> func);
};

struct EmuApplication
{
    std::unique_ptr<QApplication> qtapp;
    std::unique_ptr<EmuConfig> config;
    std::unique_ptr<SDLInputManager> input_manager;
    std::unique_ptr<EmuMainWindow> window;
    std::unique_ptr<S9xSoundDriver> sound_driver;
    std::unique_ptr<EmuThread> emu_thread;
    Snes9xController *core;

    EmuApplication();
    ~EmuApplication();
    bool openFile(std::string filename);
    void handleBinding(std::string name, bool pressed);
    void updateSettings();
    void updateBindings();
    bool isBound(EmuBinding b);
    void reportBinding(EmuBinding b, bool active);
    void startInputTimer();
    void pollJoysticks();
    void restartAudio();
    void writeSamples(int16_t *data, int samples);
    void mainLoop();
    void pause();
    void reset();
    void powerCycle();
    void suspendThread();
    void unsuspendThread();
    bool isPaused();
    void unpause();
    void loadState(int slot);
    void loadState(std::string filename);
    void saveState(int slot);
    void saveState(std::string filename);
    std::string getStateFolder();
    void loadUndoState();
    void startGame();
    void startThread();
    void stopThread();
    bool isCoreActive();

    enum Handler
    {
        Core = 0,
        UI   = 1
    };
    std::map<uint32_t, std::pair<std::string, Handler>> bindings;
    std::unique_ptr<QTimer> poll_input_timer;
    std::function<void(EmuBinding)> binding_callback = nullptr;
    std::function<void()> joypads_changed_callback = nullptr;
    int save_slot = 0;
    int pause_count = 0;
    int suspend_count = 0;
};