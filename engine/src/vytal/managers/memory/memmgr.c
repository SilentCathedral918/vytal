#include "memmgr.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"

#define not_enough_mem(target, mem_size) ((target->_used_mem + mem_size) > target->_capacity)
#define PROGRESS_BAR_LENGTH 50

typedef struct Memory_Manager_Allocator_Map_Entry {
    Str             _id;
    VoidPtr         _allocator;
    MemMgrAllocType _type;
} MemMgrAllocatorMapEntry;

typedef struct Memory_Manager_Allocator_Map {
    MemMgrAllocatorMapEntry *_entries;
    ByteSize                 _count;
    ByteSize                 _capacity;
} MemMgrAllocatorMap;

typedef struct Memory_Manager_State {
    MemMgrAllocatorMap _alloc_map;
    ByteSize           _used_mem;
    UInt64             _num_allocs;
    Bool               _initialized;
} Memory_Manager_State;

typedef struct Memory_Manager_Tag_Info {
    ConstStr        _id;
    MemMgrAllocType _type;
    ByteSize        _capacity;
} MemMgrTagInfo;
static const MemMgrTagInfo memory_tag_infos[] = {
    {"Application", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(16)}, {"Physics", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(16)},
    {"AI", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(8)},           {"Audio", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(8)},
    {"Network", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(4)},      {"Core Game Logic", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(16)},
    {"Game Entity", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(32)}, {"Game Scene", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(64)},
    {"Containers", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(4)},   {"Delegates", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(2)},
    {"Renderer", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(32)},    {"Resources", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(128)}};

static Memory_Manager_State *state;

void _memory_manager_allocmap_construct(void) {
    state->_alloc_map._count = MEMORY_TAGS_TOTAL;

    ByteSize capacity_ = 10;
    for (; capacity_ < MEMORY_TAGS_TOTAL; capacity_ *= 2) {
    }
    state->_alloc_map._capacity = capacity_;

    state->_alloc_map._entries = hal_mem_malloc(sizeof(MemMgrAllocatorMapEntry) * capacity_);
    for (ByteSize i = 0; i < MEMORY_TAGS_TOTAL; ++i) {
        MemMgrAllocatorMapEntry *entry_    = &(state->_alloc_map._entries[i]);
        const MemMgrTagInfo      tag_info_ = memory_tag_infos[i];

        entry_->_id = strdup(tag_info_._id);

        entry_->_type = tag_info_._type;

        switch (tag_info_._type) {
        case ALLOCTYPE_ARENA:
            entry_->_allocator = allocator_arena_construct(tag_info_._capacity);
            break;

        default:
            hal_mem_free(entry_->_id);
            entry_->_id = NULL;
            break;
        }
    }
}

void _memory_manager_allocmap_destruct(void) {
    for (ByteSize i = 0; i < MEMORY_TAGS_TOTAL; ++i) {
        MemMgrAllocatorMapEntry *entry_    = &(state->_alloc_map._entries[i]);
        const MemMgrTagInfo      tag_info_ = memory_tag_infos[i];

        hal_mem_free(entry_->_id);

        switch (tag_info_._type) {
        case ALLOCTYPE_ARENA:
            allocator_arena_destruct(entry_->_allocator);

        default:
            break;
        }

        hal_mem_memzero(&(state->_alloc_map._entries[i]), sizeof(MemMgrAllocatorMapEntry));
    }

    hal_mem_free(state->_alloc_map._entries);
    hal_mem_memzero(&(state->_alloc_map), sizeof(MemMgrAllocatorMap));
}

void memory_manager_startup(MemoryManager *manager) {
    // allocate internal state
    state = hal_mem_malloc(sizeof(Memory_Manager_State));

    // init internal state
    {
        state->_used_mem   = 0;
        state->_num_allocs = 0;
        _memory_manager_allocmap_construct();

        state->_initialized = true;
    }

    // assign internal state to manager ownership
    manager->_internal_state = state;
}

void memory_manager_shutdown(MemoryManager *manager) {
    if (!state || !state->_initialized)
        return;

    // free internal state
    {
        // members
        _memory_manager_allocmap_destruct();
        hal_mem_memzero(state, sizeof(Memory_Manager_State));

        // self
        hal_mem_free(state);
    }

    // release from manager ownership
    manager->_internal_state = NULL;
}

VT_INLINE void _memory_manager_report_print_id_allocname(ConstStr id, ConstStr alloc_name) {
    misc_console_setforeground_rgb(0, 255, 0);
    misc_console_write(id);
    misc_console_reset();
    misc_console_write(" (");
    misc_console_setforeground_rgb(255, 255, 255);
    misc_console_write("Allocator: %s", alloc_name);
    misc_console_reset();
    misc_console_write(")\n");
}

void _memory_manager_report_print_use_percentage(const ByteSize used_mem, const ByteSize capacity, const UInt16 meter_length) {
    misc_console_reset();
    misc_console_setforeground_rgb(240, 255, 0);

    misc_console_write("[");

    Flt32  percentage_       = (Flt32)used_mem / (Flt32)capacity;
    UInt32 meter_percentage_ = (UInt32)(percentage_ * meter_length);

    if (percentage_ > .8f)
        misc_console_setforeground_rgb(253, 58, 45);
    else if (percentage_ > .6f)
        misc_console_setforeground_rgb(254, 97, 44);
    else if (percentage_ > .4f)
        misc_console_setforeground_rgb(255, 154, 0);
    else if (percentage_ > .2f)
        misc_console_setforeground_rgb(255, 206, 0);
    else
        misc_console_setforeground_rgb(240, 255, 0);

    for (UInt16 i = 0; i < meter_length; ++i)
        misc_console_write(i < meter_percentage_ ? "=" : " ");

    misc_console_reset();
    misc_console_setforeground_rgb(240, 255, 0);
    misc_console_write("] %u %%\n\n", (UInt32)(percentage_ * 100));
    misc_console_reset();
}

void memory_manager_report(const MemoryManager *manager) {
    if (!manager || !state || !state->_initialized) {
        misc_console_setforeground_rgb(255, 0, 0);
        misc_console_writeln("Memory Manager is not initialized.");
        misc_console_reset();
        return;
    }

    // title
    misc_console_setbackground_rgb(0, 255, 255);
    misc_console_writeln("------------------- MEMORY USAGE -------------------");
    misc_console_reset();

    // memory tags
    {
        for (ByteSize i = 0; i < state->_alloc_map._count; ++i) {
            MemMgrAllocatorMapEntry *entry_    = &(state->_alloc_map._entries[i]);
            ByteSize                 used_     = 0;
            ByteSize                 capacity_ = 0;

            switch (entry_->_type) {
            case ALLOCTYPE_ARENA:
                ArenaAllocator arena_ = VT_CAST(ArenaAllocator, entry_->_allocator);
                used_                 = arena_->_used_mem;
                capacity_             = arena_->_capacity;
                _memory_manager_report_print_id_allocname(entry_->_id, allocator_arena_getname());
                break;

            default:
                continue;
            }

            // Print progress bar
            _memory_manager_report_print_use_percentage(used_, capacity_, PROGRESS_BAR_LENGTH);
        }
    }

    misc_console_reset();
}
