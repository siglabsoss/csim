#include <core/sigworld.hpp>
#include <core/logger.hpp>

int main(int argc, char *argv[])
{
    log_info("Starting RetroSim!");

    SigWorld world;

    radio_config_t config;

    for (int i = 0; i < 10; i++) {
        config.position = Vector2d(static_cast<double>(i), static_cast<double>(i));
        world.addRadio(config);
    }

    world.init();

    return 0;
}
