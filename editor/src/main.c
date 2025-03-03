#include <stdio.h>

#include "core/editor/editor.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: vytal_editor <path/to/vtproj>\n");
        return 1;
    }

    EditorResult result_;

    // startup
    {
        Str project_filepath_ = argv[1];

        result_ = editor_startup(project_filepath_);
        if (result_ != EDITOR_SUCCESS)
            return result_;
    }

    // update
    {
        result_ = editor_update();
        if (result_ != EDITOR_SUCCESS)
            return result_;
    }

    // shutdown
    {
        result_ = editor_shutdown();
        if (result_ != EDITOR_SUCCESS)
            return result_;
    }

    return 0;
}
