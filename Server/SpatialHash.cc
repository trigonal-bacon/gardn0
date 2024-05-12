#include <Server/SpatialHash.hh>

#include <Server/Process/Process.hh>

#include <Shared/Simulation.hh>
#include <Shared/Entity.hh>

SpatialHash::SpatialHash(Simulation *sim) : simulation(sim) { clear(); }

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

void SpatialHash::collide() {
    for (uint32_t x = 0; x < MAX_GRID_X; ++x) {
        for (uint32_t y = 0; y < MAX_GRID_Y; ++y) {
            StaticArray<EntityId, CELL_CAPACITY> &cell = cells[x][y];
            for (uint32_t i = 0; i < cell.length; ++i) {
                for (uint32_t j = i + 1; j < cell.length; ++j) on_collide(simulation, simulation->get_ent(cell[i]), simulation->get_ent(cell[j]));
                if (x < MAX_GRID_X - 1) {
                    if (y > 0) {
                        StaticArray<EntityId, CELL_CAPACITY> &cell2 = cells[x+1][y-1];
                        for (uint32_t j = 0; j < cell2.length; ++j) on_collide(simulation, simulation->get_ent(cell[i]), simulation->get_ent(cell2[j]));
                    }
                    if (1) {
                        StaticArray<EntityId, CELL_CAPACITY> &cell2 = cells[x+1][y];
                        for (uint32_t j = 0; j < cell2.length; ++j) on_collide(simulation, simulation->get_ent(cell[i]), simulation->get_ent(cell2[j]));
                    }
                    if (y < MAX_GRID_Y - 1) {
                        StaticArray<EntityId, CELL_CAPACITY> &cell2 = cells[x+1][y+1];
                        for (uint32_t j = 0; j < cell2.length; ++j) on_collide(simulation, simulation->get_ent(cell[i]), simulation->get_ent(cell2[j]));
                    }
                }
                if (y < MAX_GRID_Y - 1) {
                    StaticArray<EntityId, CELL_CAPACITY> &cell2 = cells[x][y+1];
                    for (uint32_t j = 0; j < cell2.length; ++j) on_collide(simulation, simulation->get_ent(cell[i]), simulation->get_ent(cell2[j]));
                }
            }
        }
    }
}


void SpatialHash::query(float x, float y, float w, float h, std::function<void(Simulation *, Entity &)> cb) {
    uint32_t sx = fclamp(x - w - GRID_SIZE, 0, ARENA_WIDTH - 1) / GRID_SIZE;
    uint32_t sy = fclamp(y - h - GRID_SIZE, 0, ARENA_HEIGHT - 1) / GRID_SIZE;
    uint32_t ex = fclamp(x + w + GRID_SIZE, 0, ARENA_WIDTH - 1) / GRID_SIZE;
    uint32_t ey = fclamp(y + h + GRID_SIZE, 0, ARENA_HEIGHT - 1) / GRID_SIZE;
    for (uint32_t _x = sx; _x <= ex; ++_x) {
        for (uint32_t _y = sy; _y <= ey; ++_y) {
            StaticArray<EntityId, CELL_CAPACITY> &cell = cells[_x][_y];
            for (uint32_t i = 0; i < cell.length; ++i) {
                Entity &ent = simulation->get_ent(cell[i]);
                if (ent.x + ent.radius < x - w) continue;
                if (ent.x - ent.radius > x + w) continue;
                if (ent.y + ent.radius < y - h) continue;
                if (ent.y - ent.radius > y + h) continue;
                cb(simulation, ent);
            }
        }
    }
}