/*
 * Copyright © 2010-2011 Linaro Limited
 *
 * This file is part of the glmark2 OpenGL (ES) 2.0 benchmark.
 *
 * glmark2 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * glmark2 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * glmark2.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *  Alexandros Frantzis (glmark2)
 */
#include "scene.h"
#include "mat.h"
#include "stack.h"
#include "vec.h"
#include "log.h"
#include "util.h"

SceneGrid::SceneGrid(Canvas &pCanvas, const std::string &name) :
    Scene(pCanvas, name)
{
    options_["grid-size"] = Scene::Option("grid-size", "32",
            "The number of squares per side of the grid (controls the number of vertices)");
    options_["grid-length"] = Scene::Option("grid-length", "5.0",
            "The length of each side of the grid (normalized) (controls the area drawn to)");
}

SceneGrid::~SceneGrid()
{
}

int
SceneGrid::load()
{
    rotationSpeed_ = 36.0f;
    running_ = false;

    return 1;
}

void
SceneGrid::unload()
{
}

void
SceneGrid::setup()
{
    Scene::setup();

    int grid_size(Util::fromString<int>(options_["grid-size"].value));
    double grid_length(Util::fromString<double>(options_["grid-length"].value));

    /* Create and configure the grid mesh */
    std::vector<int> vertex_format;
    vertex_format.push_back(3);
    mesh_.set_vertex_format(vertex_format);

    /*
     * The spacing needed in order for the area of the requested grid
     * to be the same as the area of a grid with size 32 and spacing 0.02.
     */
    double spacing = grid_length * (1 - 4.38 / 5.0) / (grid_size - 1.0);

    mesh_.make_grid(grid_size, grid_size, grid_length, grid_length,
                    grid_size > 1 ? spacing : 0);
    mesh_.build_vbo();

    currentFrame_ = 0;
    rotation_ = 0.0f;
}

void
SceneGrid::teardown()
{
    program_.stop();
    program_.release();
    mesh_.reset();

    Scene::teardown();
}

void
SceneGrid::update()
{
    double current_time = Scene::get_timestamp_us() / 1000000.0;
    double dt = current_time - lastUpdateTime_;
    double elapsed_time = current_time - startTime_;

    lastUpdateTime_ = current_time;

    if (elapsed_time >= duration_) {
        averageFPS_ = currentFrame_ / elapsed_time;
        running_ = false;
    }

    rotation_ += rotationSpeed_ * dt;

    currentFrame_++;
}

void
SceneGrid::draw()
{
    // Load the ModelViewProjectionMatrix uniform in the shader
    LibMatrix::Stack4 model_view;
    LibMatrix::mat4 model_view_proj(canvas_.projection());

    model_view.translate(0.0f, 0.0f, -5.0f);
    model_view.rotate(rotation_, 0.0f, 0.0f, 1.0f);
    model_view_proj *= model_view.getCurrent();

    program_["ModelViewProjectionMatrix"] = model_view_proj;

    mesh_.render_vbo();
}

Scene::ValidationResult
SceneGrid::validate()
{
    return Scene::ValidationUnknown;
}
