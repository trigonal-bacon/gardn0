#include <Server/SpatialHash.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

SpatialHash::SpatialHash(Simulation *sim) : simulation(sim) {}

void SpatialHash::clear() {
    for (uint32_t x = 0; x < MAX_GRID_X; ++x)
        for (uint32_t y = 0; y < MAX_GRID_Y; ++y)
            cells[x][y].clear();
}

void SpatialHash::insert(Entity &ent) {
    assert(ent.has_component(kPhysics));
    uint32_t x = fclamp(ent.x, 0, ARENA_WIDTH - 1) / GRID_SIZE;
    uint32_t y = fclamp(ent.y, 0, ARENA_HEIGHT - 1) / GRID_SIZE;
    cells[x][y].push(ent.id);
}

void SpatialHash::query(float x, float y, float w, float h, std::function<void(Simulation *, Entity &)> cb) {
    uint32_t sx = fclamp(x - w, 0, ARENA_WIDTH - 1) / GRID_SIZE;
    uint32_t sy = fclamp(y - h, 0, ARENA_HEIGHT - 1) / GRID_SIZE;
    uint32_t ex = fclamp(x + w, 0, ARENA_WIDTH - 1) / GRID_SIZE;
    uint32_t ey = fclamp(y + h, 0, ARENA_HEIGHT - 1) / GRID_SIZE;
    for (uint32_t x = sx; x <= ex; ++x) {
        for (uint32_t y = sy; y <= ey; ++y) {
            StaticArray<EntityId, CELL_CAPACITY> &cell = cells[x][y];
            for (uint32_t i = 0; i < cell.length; ++i)
                cb(simulation, simulation->get_ent(cell[i]));
        }
    }
}