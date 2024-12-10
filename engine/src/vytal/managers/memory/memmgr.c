#include "memmgr.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/memory/allocators/pool.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"

#define not_enough_mem(target, mem_size) ((target->_used_mem + mem_size) > target->_capacity)
#define PROGRESS_BAR_LENGTH 50
#define POOL_ALLOC_SYSTEM_DEFAULT_CHUNK_COUNT 256

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
    {"Containers", ALLOCTYPE_POOL, VT_SIZE_MB_MULT(4)},    {"Delegates", ALLOCTYPE_ARENA, VT_SIZE_MB_MULT(2)},
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

        entry_->_id = misc_str_strdup(tag_info_._id);

        entry_->_type = tag_info_._type;

        switch (tag_info_._type) {
        case ALLOCTYPE_ARENA:
            entry_->_allocator = allocator_arena_construct(tag_info_._capacity);
            break;

        case ALLOCTYPE_POOL:
            entry_->_allocator = allocator_pool_construct(tag_info_._capacity, POOL_ALLOC_SYSTEM_DEFAULT_CHUNK_COUNT);
            break;

        default:
            hal_mem_free(entry_->_id);
            entry_->_id = NULL;
            break;
        }
    }
}

void _memory_manager_allocmap_destruct(void) {
    for (ByteSize i = 0; i < state->_alloc_map._count; ++i) {
        MemMgrAllocatorMapEntry *entry_ = &(state->_alloc_map._entries[i]);

        switch (memory_tag_infos[i]._type) {
        case ALLOCTYPE_ARENA:
            allocator_arena_destruct(entry_->_allocator);
            break;

        case ALLOCTYPE_POOL:
            allocator_pool_destruct(entry_->_allocator);
            break;

        default:
            break;
        }

        hal_mem_free(entry_->_id);
        hal_mem_memzero(&(state->_alloc_map._entries[i]), sizeof(MemMgrAllocatorMapEntry));
    }

    hal_mem_free(state->_alloc_map._entries);
    hal_mem_memzero(&(state->_alloc_map), sizeof(MemMgrAllocatorMap));
}

MemMgrAllocatorMapEntry *_memory_manager_allocmap_insert_arena(ConstStr id, const ByteSize capacity) {
    MemMgrAllocatorMap *map_ = &(state->_alloc_map);

    UInt64 return_idx_ = map_->_count;

    // resize if needed
    if (map_->_count >= map_->_capacity) {
        ByteSize                 new_capacity_ = map_->_capacity * 2;
        MemMgrAllocatorMapEntry *new_entries_ =
            hal_mem_realloc(map_->_entries, sizeof(MemMgrAllocatorMapEntry) * new_capacity_);

        if (!new_entries_)
            return NULL;

        map_->_entries  = new_entries_;
        map_->_capacity = new_capacity_;
    }

    // insert new entry
    {
        map_->_entries[map_->_count]._id        = misc_str_strdup(id);
        map_->_entries[map_->_count]._type      = ALLOCTYPE_ARENA;
        map_->_entries[map_->_count]._allocator = allocator_arena_construct(capacity);

        map_->_count++;
    }

    return &(map_->_entries[return_idx_]);
}

MemMgrAllocatorMapEntry *_memory_manager_allocmap_insert_pool(ConstStr id, const ByteSize capacity,
                                                              const ByteSize chunk_count) {
    MemMgrAllocatorMap *map_ = &(state->_alloc_map);

    UInt64 return_idx_ = map_->_count;

    // resize if needed
    if (map_->_count >= map_->_capacity) {
        ByteSize                 new_capacity_ = map_->_capacity * 2;
        MemMgrAllocatorMapEntry *new_entries_ =
            hal_mem_realloc(map_->_entries, sizeof(MemMgrAllocatorMapEntry) * new_capacity_);

        if (!new_entries_)
            return NULL;

        map_->_entries  = new_entries_;
        map_->_capacity = new_capacity_;
    }

    // insert new entry
    {
        map_->_entries[map_->_count]._id        = misc_str_strdup(id);
        map_->_entries[map_->_count]._type      = ALLOCTYPE_POOL;
        map_->_entries[map_->_count]._allocator = allocator_pool_construct(capacity, chunk_count);

        map_->_count++;
    }

    return &(map_->_entries[return_idx_]);
}

Bool _memory_manager_allocmap_remove(ConstStr id) {
    MemMgrAllocatorMap *map_ = &(state->_alloc_map);

    if (!map_ || !id)
        return false;

    for (ByteSize i = 0; i < map_->_count; ++i) {
        if (misc_str_strcmp(map_->_entries[i]._id, id, true) == 0) {
            // free the entry
            {
                switch (map_->_entries[i]._type) {
                case ALLOCTYPE_ARENA:
                    allocator_arena_destruct(map_->_entries[i]._allocator);
                    break;

                case ALLOCTYPE_POOL:
                    allocator_pool_destruct(map_->_entries[i]._allocator);
                    break;

                default:
                    return false;
                }

                hal_mem_free(map_->_entries[i]._id);
                hal_mem_memzero(&(map_->_entries[i]), sizeof(MemMgrAllocatorMapEntry));
            }

            // shift trailing entries to fill the gap
            {
                for (ByteSize j = i; j < map_->_count - 1; ++j)
                    map_->_entries[j] = map_->_entries[j + 1];
            }

            hal_mem_memzero(&(map_->_entries[map_->_count]), sizeof(MemMgrAllocatorMapEntry));
            map_->_count--;
            return true;
        }
    }

    return false;
}

MemMgrAllocatorMapEntry *_memory_manager_allocmap_find(ConstStr id) {
    MemMgrAllocatorMap *map_ = &(state->_alloc_map);

    if (!id || !map_)
        return NULL;

    for (ByteSize i = 0; i < map_->_count; ++i) {
        if (misc_str_strcmp(map_->_entries[i]._id, id, true) == 0) {
            return &(map_->_entries[i]);
        }
    }

    return NULL;
}

void memory_manager_startup(void) {
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
    memory_manager._internal_state = state;
}

void memory_manager_shutdown(void) {
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
    memory_manager._internal_state = NULL;
}

VoidPtr memory_manager_allocate(const ByteSize size, const MemoryTag tag) {
    if (!state || !state->_initialized)
        return NULL;

    MemMgrAllocatorMap      *map_   = &(state->_alloc_map);
    MemMgrAllocatorMapEntry *entry_ = &(map_->_entries[tag]);

    switch (entry_->_type) {
    case ALLOCTYPE_ARENA:
        return allocator_arena_allocate(entry_->_allocator, size);

    case ALLOCTYPE_POOL:
        return allocator_pool_allocate(entry_->_allocator);

    default:
        break;
    }

    return NULL;
}

Bool memory_manager_deallocate(VoidPtr ptr, const MemoryTag tag) {
    if (!state || !state->_initialized || !ptr)
        return false;

    MemMgrAllocatorMap      *map_   = &(state->_alloc_map);
    MemMgrAllocatorMapEntry *entry_ = &(map_->_entries[tag]);

    switch (entry_->_type) {
    case ALLOCTYPE_ARENA:
        // no-op - arena allocators don't do individual deallocations
        return false;

    case ALLOCTYPE_POOL:
        return allocator_pool_deallocate(entry_->_allocator, ptr);

    default:
        break;
    }

    return false;
}

VoidPtr memory_manager_construct_allocator_arena(ConstStr id, const ByteSize capacity) {
    MemMgrAllocatorMapEntry *constructed_entry_ = _memory_manager_allocmap_insert_arena(id, capacity);
    if (!constructed_entry_)
        return NULL;

    return constructed_entry_->_allocator;
}

VoidPtr memory_manager_construct_allocator_pool(ConstStr id, const ByteSize capacity, const ByteSize chunk_count) {
    MemMgrAllocatorMapEntry *constructed_entry_ = _memory_manager_allocmap_insert_pool(id, capacity, chunk_count);
    if (!constructed_entry_)
        return NULL;

    return constructed_entry_->_allocator;
}

Bool memory_manager_destruct_allocator(ConstStr id) { return _memory_manager_allocmap_remove(id); }

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

ConstStr _memory_manager_report_format_memory_size(const ByteSize memory_size) {
    static Char vt_memory_manager_memory_format_[16];

    if (memory_size < VT_SIZE_KB_IN_BYTES)
        snprintf(vt_memory_manager_memory_format_, sizeof vt_memory_manager_memory_format_, "%lld B", memory_size);

    else if (memory_size < VT_SIZE_MB_IN_BYTES)
        snprintf(vt_memory_manager_memory_format_, sizeof vt_memory_manager_memory_format_, "%.2f KB",
                 VT_CAST(Flt32, memory_size) / VT_CAST(Flt32, VT_SIZE_KB_IN_BYTES));

    else if (memory_size < VT_SIZE_GB_IN_BYTES)
        snprintf(vt_memory_manager_memory_format_, sizeof vt_memory_manager_memory_format_, "%.2f MB",
                 VT_CAST(Flt32, memory_size) / VT_CAST(Flt32, VT_SIZE_MB_IN_BYTES));

    else
        snprintf(vt_memory_manager_memory_format_, sizeof vt_memory_manager_memory_format_, "%.2f GB",
                 VT_CAST(Flt32, memory_size) / VT_CAST(Flt32, VT_SIZE_GB_IN_BYTES));

    return vt_memory_manager_memory_format_;
}

void _memory_manager_report_print_use_percentage(const ByteSize used_mem, const ByteSize capacity, const UInt16 meter_length) {
    misc_console_reset();
    misc_console_setforeground_rgb(240, 255, 0);

    misc_console_write("[");

    Flt32  percentage_       = VT_CAST(Flt32, used_mem) / VT_CAST(Flt32, capacity);
    UInt32 meter_percentage_ = VT_CAST(UInt32, percentage_ * meter_length);

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

    Char fmt_used_mem_[16];
    strcpy(fmt_used_mem_, _memory_manager_report_format_memory_size(used_mem));
    Char fmt_capacity_[16];
    strcpy(fmt_capacity_, _memory_manager_report_format_memory_size(capacity));

    misc_console_reset();
    misc_console_setforeground_rgb(240, 255, 0);
    misc_console_write("] %.4f %% (%s / %s)\n\n", percentage_ * 100, fmt_used_mem_, fmt_capacity_);
    misc_console_reset();
}

Bool memory_manager_report(const ConstStr id) {
    if (!state || !state->_initialized) {
        misc_console_setforeground_rgb(255, 0, 0);
        misc_console_writeln("Memory Manager is not initialized.");
        misc_console_reset();
        return false;
    }

    MemMgrAllocatorMapEntry *entry_    = _memory_manager_allocmap_find(id);
    ByteSize                 used_     = 0;
    ByteSize                 capacity_ = 0;

    // no entry with specified id
    if (!entry_)
        return false;

    // title
    {
        misc_console_setforeground_rgb(22, 222, 122);
        misc_console_writeln("Memory Usage (%s)", id);

        Char     sep_[64]      = {'\0'};
        ByteSize title_length_ = misc_str_strlen("Memory Usage");
        hal_mem_memset(sep_, '-', title_length_ + misc_str_strlen(id) + 3);
        misc_console_writeln(sep_);

        misc_console_reset();
    }

    switch (entry_->_type) {
    case ALLOCTYPE_ARENA:
        ArenaAllocator arena_ = VT_CAST(ArenaAllocator, entry_->_allocator);
        used_                 = allocator_arena_usedmem(arena_);
        capacity_             = allocator_arena_capacity(arena_);

        _memory_manager_report_print_id_allocname(entry_->_id, allocator_arena_getname());
        break;

    case ALLOCTYPE_POOL:
        PoolAllocator pool_ = VT_CAST(PoolAllocator, entry_->_allocator);
        used_               = allocator_pool_usedmem(pool_);
        capacity_           = allocator_pool_capacity(pool_);

        _memory_manager_report_print_id_allocname(entry_->_id, allocator_pool_getname());
        break;

    default:
        return false;
    }

    // Print progress bar
    _memory_manager_report_print_use_percentage(used_, capacity_, PROGRESS_BAR_LENGTH);

    return true;
}

void memory_manager_report_all(void) {
    if (!state || !state->_initialized) {
        misc_console_setforeground_rgb(255, 0, 0);
        misc_console_writeln("Memory Manager is not initialized.");
        misc_console_reset();
        return;
    }

    // title
    {
        misc_console_setforeground_rgb(22, 222, 122);
        misc_console_writeln("Memory Usage");
        misc_console_writeln("------------");
        misc_console_reset();
    }

    // memory tags
    {
        for (ByteSize i = 0; i < state->_alloc_map._count; ++i) {
            MemMgrAllocatorMapEntry *entry_    = &(state->_alloc_map._entries[i]);
            ByteSize                 used_     = 0;
            ByteSize                 capacity_ = 0;

            switch (entry_->_type) {
            case ALLOCTYPE_ARENA:
                ArenaAllocator arena_ = VT_CAST(ArenaAllocator, entry_->_allocator);
                used_                 = allocator_arena_usedmem(arena_);
                capacity_             = allocator_arena_capacity(arena_);

                _memory_manager_report_print_id_allocname(entry_->_id, allocator_arena_getname());
                break;

            case ALLOCTYPE_POOL:
                PoolAllocator pool_ = VT_CAST(PoolAllocator, entry_->_allocator);
                used_               = allocator_pool_usedmem(pool_);
                capacity_           = allocator_pool_capacity(pool_);

                _memory_manager_report_print_id_allocname(entry_->_id, allocator_pool_getname());
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
