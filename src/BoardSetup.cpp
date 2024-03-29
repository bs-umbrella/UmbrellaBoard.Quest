#include "BoardSetup.hpp"

DEFINE_TYPE(Umbrella, BoardSetup);

namespace Umbrella {
    void BoardSetup::ctor(GlobalNamespace::MainFlowCoordinator* mainFlowCoordinator, BoardViewController* boardView) {
        _mainFlowCoordinator = mainFlowCoordinator;
        _boardView = boardView;
    }

    void BoardSetup::Initialize() {
        _mainFlowCoordinator->_providedRightScreenViewController = _boardView;
    }

    void BoardSetup::Dispose() {
        _mainFlowCoordinator->_providedRightScreenViewController = nullptr;
    }
}
