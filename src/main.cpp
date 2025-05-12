#include "Pomo.hpp"

int main (int argc, char *argv[]) {
    QApplication app(argc, argv);
    Pomo pomo;
    pomo.show();
    app.exec();
}
