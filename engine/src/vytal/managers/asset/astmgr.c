#include "astmgr.h"

#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"

#include "vytal/managers/memory/memmgr.h"

typedef struct Asset_Manager_State {
    Map _image_map;
} AssetManagerState;

static AssetManagerState *state;

void asset_manager_startup(void) {
    // allocate internal state
    state = memory_manager_allocate(sizeof(AssetManagerState), MEMORY_TAG_MODULE);

    // init internal state
    {
        state->_image_map = container_map_construct(sizeof(Image));
    }

    // assign internal state to manager ownership
    asset_manager._internal_state = state;
}

void asset_manager_shutdown(void) {
    if (!state)
        return;

    // free internal state
    {
        // images
        container_map_destruct(state->_image_map);

        // self
        hal_mem_memzero(state, sizeof(AssetManagerState));
        memory_manager_deallocate(state, MEMORY_TAG_MODULE);
    }

    // release from manager ownership
    asset_manager._internal_state = NULL;
}
