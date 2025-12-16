#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <vector>
#include <memory>
#include <string>
#include "../cpp_core/include/game_environment.h"
#include "../cpp_core/include/types.h"

namespace py = pybind11;

// Position 包装类
struct PyPosition
{
    int x, y;

    PyPosition() : x(0), y(0) {}
    PyPosition(int x, int y) : x(x), y(y) {}
    PyPosition(const Position &pos) : x(pos.x), y(pos.y) {}

    bool operator==(const PyPosition &other) const
    {
        return x == other.x && y == other.y;
    }

    // 转换为原始 Position
    Position to_position() const
    {
        return Position(x, y);
    }
};

// GameEnvironment 包装类
class PyGameEnvironment
{
private:
    std::unique_ptr<GameEnvironment> env_;

public:
    PyGameEnvironment(int width, int height, const std::string &config_file = "config.txt")
        : env_(std::make_unique<GameEnvironment>(width, height, config_file)) {}

    void initialize_random(int num_cells)
    {
        env_->initializeRandom(num_cells);
    }

    void remove_cell(int x, int y)
    {
        env_->removeCell(Position(x, y));
    }

    void set_cell(int x, int y)
    {
        env_->setCell(Position(x, y));
    }

    void update()
    {
        env_->update();
    }

    void update_with_moves(py::list moves)
    {
        std::vector<int> moves_vec;
        for (auto item : moves)
        {
            moves_vec.push_back(item.cast<int>());
        }
        env_->updateWithMoves(moves_vec);
    }

    py::array_t<float> get_cell_states()
    {
        auto states = env_->getCellStates();

        if (states.empty())
        {
            return py::array_t<float>({0, 0});
        }

        // 创建 numpy 数组
        size_t rows = states.size();
        size_t cols = states[0].size();

        auto result = py::array_t<float>({rows, cols});
        auto buffer = result.mutable_unchecked<2>();

        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                buffer(i, j) = states[i][j];
            }
        }

        return result;
    }

    py::list get_cells()
    {
        const auto &cells = env_->getCells();
        py::list positions;

        for (const auto &cell : cells)
        {
            if (cell->isAlive())
            {
                py::dict cell_info;
                cell_info["id"] = cell->getId();
                cell_info["x"] = cell->getPosition().x;
                cell_info["y"] = cell->getPosition().y;
                cell_info["age"] = cell->getAge();
                cell_info["energy"] = cell->getEnergy();
                positions.append(cell_info);
            }
        }

        return positions;
    }

    int get_population()
    {
        return env_->getPopulation();
    }

    double get_density()
    {
        return env_->getDensity();
    }

    int get_width()
    {
        return env_->getWidth();
    }

    int get_height()
    {
        return env_->getHeight();
    }

    void reload_config()
    {
        env_->reloadConfig();
    }

    void print_config()
    {
        env_->printConfig();
    }

    // 获取网格状态（用于可视化）
    py::array_t<bool> get_grid_state()
    {
        auto grid = env_->getGridState();

        if (grid.empty())
        {
            return py::array_t<bool>({0, 0});
        }

        size_t rows = grid.size();
        size_t cols = grid[0].size();

        auto result = py::array_t<bool>({rows, cols});
        auto buffer = result.mutable_unchecked<2>();

        for (size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                buffer(i, j) = grid[i][j];
            }
        }

        return result;
    }

    // 获取空邻居位置
    py::list get_empty_neighbors(int x, int y, int d = -1)
    {
        Position pos(x, y);
        auto empty_positions = env_->getEmptyNeighbors(pos, d);

        py::list result;
        for (const auto &pos : empty_positions)
        {
            result.append(py::make_tuple(pos.x, pos.y));
        }

        return result;
    }

    // 检查位置是否有效
    bool is_valid_position(int x, int y)
    {
        return env_->isValidPosition(Position(x, y));
    }

    // 检查位置是否为空
    bool is_position_empty(int x, int y)
    {
        return env_->isPositionEmpty(Position(x, y));
    }
};

PYBIND11_MODULE(smart_life_core, m)
{
    m.doc() = "Smart Game of Life - PyBind11 Bindings";

    // 绑定 Position 类
    py::class_<PyPosition>(m, "Position")
        .def(py::init<>())
        .def(py::init<int, int>())
        .def_readwrite("x", &PyPosition::x)
        .def_readwrite("y", &PyPosition::y)
        .def("__eq__", &PyPosition::operator==)
        .def("__repr__", [](const PyPosition &p)
             { return "Position(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")"; });

    // 绑定 GameEnvironment 类
    py::class_<PyGameEnvironment>(m, "GameEnvironment")
        .def(py::init<int, int>(),
             py::arg("width"), py::arg("height"),
             "Create a new game environment with specified dimensions")
        .def(py::init<int, int, std::string>(),
             py::arg("width"), py::arg("height"), py::arg("config_file"),
             "Create a new game environment with config file")
        .def("initialize_random", &PyGameEnvironment::initialize_random,
             py::arg("num_cells"),
             "Initialize the environment with random cells")
        .def("update", &PyGameEnvironment::update,
             "Update the game state (standard Conway rules)")
        .def("update_with_moves", &PyGameEnvironment::update_with_moves,
             py::arg("moves"),
             "Update the game state with cell moves")
        .def("get_cell_states", &PyGameEnvironment::get_cell_states,
             "Get the current state of all cells as a numpy array")
        .def("get_cells", &PyGameEnvironment::get_cells,
             "Get positions and info of all living cells")
        .def("get_grid_state", &PyGameEnvironment::get_grid_state,
             "Get the entire grid state as a numpy array")
        .def("get_population", &PyGameEnvironment::get_population,
             "Get the current population count")
        .def("get_density", &PyGameEnvironment::get_density,
             "Get the current population density")
        .def("get_width", &PyGameEnvironment::get_width,
             "Get the grid width")
        .def("get_height", &PyGameEnvironment::get_height,
             "Get the grid height")
        .def("reload_config", &PyGameEnvironment::reload_config,
             "Reload configuration from file")
        .def("print_config", &PyGameEnvironment::print_config,
             "Print the current configuration")
        .def("get_empty_neighbors", &PyGameEnvironment::get_empty_neighbors,
             py::arg("x"), py::arg("y"), py::arg("d") = -1,
             "Get empty neighbor positions")
        .def("is_valid_position", &PyGameEnvironment::is_valid_position,
             py::arg("x"), py::arg("y"),
             "Check if a position is valid")
        .def("is_position_empty", &PyGameEnvironment::is_position_empty,
             py::arg("x"), py::arg("y"),
             "Check if a position is empty")
        .def("remove_cell", &PyGameEnvironment::remove_cell,
             py::arg("x"), py::arg("y"),
             "Remove a cell at the specified position")
        .def("set_cell", &PyGameEnvironment::set_cell,
             py::arg("x"), py::arg("y"),
             "Set a cell at the specified position");
}
