#include <LibGUI/GWindow.h>
#include <LibGUI/GWidget.h>
#include <LibGUI/GBoxLayout.h>
#include <LibGUI/GApplication.h>
#include <LibGUI/GStatusBar.h>
#include <LibGUI/GToolBar.h>
#include <LibGUI/GMenuBar.h>
#include <LibGUI/GTextEditor.h>
#include <LibGUI/GAction.h>
#include <LibGUI/GFontDatabase.h>
#include <LibCore/CFile.h>
#include <AK/StringBuilder.h>
#include "VBForm.h"
#include "VBWidget.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
    GApplication app(argc, argv);

    auto* form1 = new VBForm("Form1");

    auto menubar = make<GMenuBar>();
    auto app_menu = make<GMenu>("Visual Builder");
    app_menu->add_action(GAction::create("Quit", { Mod_Alt, Key_F4 }, [] (const GAction&) {
        GApplication::the().quit(0);
        return;
    }));
    menubar->add_menu(move(app_menu));

    auto file_menu = make<GMenu>("File");
    menubar->add_menu(move(file_menu));

    auto edit_menu = make<GMenu>("Edit");
    menubar->add_menu(move(edit_menu));

    auto help_menu = make<GMenu>("Help");
    help_menu->add_action(GAction::create("About", [] (const GAction&) {
        dbgprintf("FIXME: Implement Help/About\n");
    }));
    menubar->add_menu(move(help_menu));

    app.set_menubar(move(menubar));

    auto* window = new GWindow;
    window->set_title(form1->name());
    window->set_rect(20, 200, 640, 400);
    window->set_main_widget(form1);
    window->set_should_exit_event_loop_on_close(true);
    window->show();

    return app.exec();
}