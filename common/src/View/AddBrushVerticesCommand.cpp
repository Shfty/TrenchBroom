/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AddBrushVerticesCommand.h"

#include "View/MapDocument.h"
#include "View/MapDocumentCommandFacade.h"

#include <kdl/string_format.h>
#include <kdl/vector_set.h>

#include <set>
#include <vector>

namespace TrenchBroom {
    namespace View {
        const Command::CommandType AddBrushVerticesCommand::Type = Command::freeType();

        AddBrushVerticesCommand::Ptr AddBrushVerticesCommand::add(const VertexToBrushesMap& vertices) {
            kdl::vector_set<Model::Brush*> allBrushes;
            for (const auto& entry : vertices) {
                const std::set<Model::Brush*>& brushes = entry.second;
                allBrushes.insert(std::begin(brushes), std::end(brushes));
            }

            const String actionName = kdl::str_plural(vertices.size(), "Add Vertex", "Add Vertices");
            return Ptr(new AddBrushVerticesCommand(Type, actionName, allBrushes.release_data(), vertices));
        }

        AddBrushVerticesCommand::AddBrushVerticesCommand(CommandType type, const String& name, const std::vector<Model::Brush*>& brushes, const VertexToBrushesMap& vertices) :
        VertexCommand(type, name, brushes),
        m_vertices(vertices) {}

        bool AddBrushVerticesCommand::doCanDoVertexOperation(const MapDocument* document) const {
            const vm::bbox3& worldBounds = document->worldBounds();
            for (const auto& entry : m_vertices) {
                const vm::vec3& position = entry.first;
                const std::set<Model::Brush*>& brushes = entry.second;
                for (const Model::Brush* brush : brushes) {
                    if (!brush->canAddVertex(worldBounds, position))
                        return false;
                }
            }
            return true;
        }

        bool AddBrushVerticesCommand::doVertexOperation(MapDocumentCommandFacade* document) {
            document->performAddVertices(m_vertices);
            return true;
        }

        bool AddBrushVerticesCommand::doCollateWith(UndoableCommand::Ptr) {
            return false;
        }
    }
}
