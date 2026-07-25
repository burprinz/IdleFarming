//
// Created by luisb on 23.06.2026.
//

#include "window.h"
#include "core/registry.h"

class FieldSystem {
public:
    static FieldSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:

    Registry* m_registry;
    Window* m_window;

    void try_to_plant(std::vector<std::vector<entt::entity>> field, entt::entity p_id) noexcept;


    inline void plant_with_radius(std::vector<std::vector<entt::entity>> field, entt::entity p_id, size_t x, size_t y, int r) noexcept {

        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];

                FarmField &f = m_registry->ecs.get<FarmField>(fieldID);

                if (f.is_empty) {
                    // Plant if field is empty and left click pressed
                    f.is_empty = false;
                    f.plant = p_id;
                    f.grow = 0.0;
                    f.harvestable = false;
                    Plant p = m_registry->ecs.get<Plant>(p_id);
                    if (p.has_inventory) {
                        Inventory inv;
                        inv.size = BigNumber(p.inventory_size);
                        inv.amount = BigNumber(0);
                        m_registry->ecs.emplace<Inventory>(fieldID, inv);
                    }
                }

            }
        }

    }

    inline void harvest_with_radius(std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r) noexcept {
        // Gets called for Every type
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];
                FarmField &f = m_registry->ecs.get<FarmField>(fieldID);

                if (!f.is_empty) {
                    Plant p = m_registry->ecs.get<Plant>(f.plant);
                    BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
                    // Try to harvest
                    if (p.has_inventory) {
                        // Machine -> Add inventory am to stash and remove plant and inv
                        Inventory inv = m_registry->ecs.get<Inventory>(fieldID);
                        am += inv.amount;
                        m_registry->ecs.remove<Inventory>(fieldID);
                        f.is_empty = true;
                    } else if (f.harvestable) {
                        // No Machine -> Add amount to stash and remove plant
                        am += p.min_collect;
                        f.harvestable = false;
                        f.is_empty = true;
                    }
                }
            }
        }
    }


    inline void collect_with_radius(std::vector<std::vector<entt::entity>> field, size_t x, size_t y, int r) noexcept {
        // Gets called for Machine or Regrowable
        size_t y_start = (static_cast<size_t>(r)>y)?0:(y-r);
        size_t x_start = (static_cast<size_t>(r)>x)?0:(x-r);

        for (size_t y_pos = y_start; y_pos < std::min<size_t>(y+r+1, field.size()); y_pos++) {
            for (size_t x_pos = x_start; x_pos < std::min<size_t>(x+r+1, field[y_pos].size()); x_pos++) {
                entt::entity fieldID = field[y_pos][x_pos];

                FarmField &f = m_registry->ecs.get<FarmField>(fieldID);

                if (!f.is_empty) {
                    Plant p = m_registry->ecs.get<Plant>(f.plant);
                    BigNumber &am = m_registry->ecs.get<Amount>(f.plant).am;
                    // Try to collect
                    if (p.has_inventory) {
                        // Machine -> Collect inventory and keep everything
                        Inventory &inv = m_registry->ecs.get<Inventory>(fieldID);
                        am += inv.amount;
                        inv.amount = BigNumber(0);
                    } else if (f.harvestable) {
                        // No Machine -> Collect and reset growtime
                        am += p.min_collect;
                        f.harvestable = false;
                        f.grow = p.growTime;
                    }
                }
            }
        }
    }

};
