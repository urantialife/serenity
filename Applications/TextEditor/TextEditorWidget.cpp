#include "TextEditorWidget.h"
#include <AK/StringBuilder.h>
#include <LibCore/CFile.h>
#include <LibGUI/GAction.h>
#include <LibGUI/GBoxLayout.h>
#include <LibGUI/GFilePicker.h>
#include <LibGUI/GFontDatabase.h>
#include <LibGUI/GMenuBar.h>
#include <LibGUI/GMessageBox.h>
#include <LibGUI/GStatusBar.h>
#include <LibGUI/GTextEditor.h>
#include <LibGUI/GToolBar.h>

TextEditorWidget::TextEditorWidget()
{
    set_layout(make<GBoxLayout>(Orientation::Vertical));
    layout()->set_spacing(0);

    auto* toolbar = new GToolBar(this);
    m_editor = new GTextEditor(GTextEditor::MultiLine, this);
    m_editor->set_ruler_visible(true);
    m_editor->set_automatic_indentation_enabled(true);
    auto* statusbar = new GStatusBar(this);

    m_editor->on_cursor_change = [statusbar, this] {
        StringBuilder builder;
        builder.appendf("Line: %d, Column: %d", m_editor->cursor().line(), m_editor->cursor().column());
        statusbar->set_text(builder.to_string());
    };

    auto new_action = GAction::create("New document", { Mod_Ctrl, Key_N }, GraphicsBitmap::load_from_file("/res/icons/16x16/new.png"), [](const GAction&) {
        dbgprintf("FIXME: Implement File/New\n");
    });

    auto open_action = GAction::create("Open document", { Mod_Ctrl, Key_O }, GraphicsBitmap::load_from_file("/res/icons/16x16/open.png"), [this](const GAction&) {
        GFilePicker picker;

        if (picker.exec() == GDialog::ExecOK) {
            m_path = picker.selected_file().string();
            open_sesame(m_path);
        }
    });

    auto save_action = GAction::create("Save document", { Mod_Ctrl, Key_S }, GraphicsBitmap::load_from_file("/res/icons/16x16/save.png"), [this](const GAction&) {
        dbgprintf("Writing document to '%s'\n", m_path.characters());
        m_editor->write_to_file(m_path);
    });

    auto menubar = make<GMenuBar>();
    auto app_menu = make<GMenu>("Text Editor");
    app_menu->add_action(GAction::create("Quit", { Mod_Alt, Key_F4 }, [](const GAction&) {
        GApplication::the().quit(0);
        return;
    }));
    menubar->add_menu(move(app_menu));

    auto file_menu = make<GMenu>("File");
    file_menu->add_action(new_action);
    file_menu->add_action(open_action);
    file_menu->add_action(save_action);
    menubar->add_menu(move(file_menu));

    auto edit_menu = make<GMenu>("Edit");
    edit_menu->add_action(m_editor->undo_action());
    edit_menu->add_action(m_editor->redo_action());
    edit_menu->add_separator();
    edit_menu->add_action(m_editor->cut_action());
    edit_menu->add_action(m_editor->copy_action());
    edit_menu->add_action(m_editor->paste_action());
    edit_menu->add_action(m_editor->delete_action());
    menubar->add_menu(move(edit_menu));

    auto font_menu = make<GMenu>("Font");
    GFontDatabase::the().for_each_fixed_width_font([&](const StringView& font_name) {
        font_menu->add_action(GAction::create(font_name, [this](const GAction& action) {
            m_editor->set_font(GFontDatabase::the().get_by_name(action.text()));
            m_editor->update();
        }));
    });
    menubar->add_menu(move(font_menu));

    auto help_menu = make<GMenu>("Help");
    help_menu->add_action(GAction::create("About", [](const GAction&) {
        dbgprintf("FIXME: Implement Help/About\n");
    }));
    menubar->add_menu(move(help_menu));

    GApplication::the().set_menubar(move(menubar));

    toolbar->add_action(move(new_action));
    toolbar->add_action(move(open_action));
    toolbar->add_action(move(save_action));

    toolbar->add_separator();

    toolbar->add_action(m_editor->cut_action());
    toolbar->add_action(m_editor->copy_action());
    toolbar->add_action(m_editor->paste_action());
    toolbar->add_action(m_editor->delete_action());

    toolbar->add_separator();

    toolbar->add_action(m_editor->undo_action());
    toolbar->add_action(m_editor->redo_action());

    m_editor->set_focus(true);
}

TextEditorWidget::~TextEditorWidget()
{
}

void TextEditorWidget::open_sesame(const String& path)
{
    dbgprintf("Our path to file in open_sesame: %s\n", path.characters());
    CFile file(path);

    if (!file.open(CIODevice::ReadOnly)) {
        GMessageBox::show(String::format("Opening \"%s\" failed: %s", path.characters(), strerror(errno)), "Error", GMessageBox::Type::Error, window());
    }

    window()->set_title(String::format("Text Editor: %s", path.characters()));
    m_editor->set_text(String::copy(file.read_all()));
}

