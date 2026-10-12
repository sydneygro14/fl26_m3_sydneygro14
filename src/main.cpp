#include "aiws/gui/main_window.hpp"

#include <QApplication>
#include <QMessageBox>

namespace {

// asks for confirmation so yes quits and no returns with the state unchanged
void confirm_exit(QWidget* parent) {
    const QMessageBox::StandardButton answer = QMessageBox::question(
        parent, "Exit", "Quit the application?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) QApplication::quit();
}

}  // namespace

int main(int argc, char* argv[]) {
    // application object must exist before any widget
    QApplication application(argc, argv);

    // window gets the exit callback before it is shown
    aiws::gui::MainWindow window;
    window.set_exit_callback(&confirm_exit);
    window.show();
    return application.exec();
}