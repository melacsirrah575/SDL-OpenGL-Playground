#include "GameManager.h"

namespace SDLFramework {

	GameManager* GameManager::sInstance = nullptr;

	GameManager* GameManager::Instance() {
		if (sInstance == nullptr) {
			sInstance = new GameManager();
		}
		return sInstance;
	}

	void GameManager::Release() {
		delete sInstance;
		sInstance = nullptr;
	}

	void GameManager::Run() {
		while (!mQuit) {
			mTimer->Update();

			while (SDL_PollEvent(&mEvent)) {
				switch (mEvent.type) {
				case SDL_QUIT:
					mQuit = true;
					break;
				case SDL_MOUSEWHEEL:
					mInputManager->MouseWheel(mEvent.wheel.y);
					break;
				}
			}

			if (mTimer->DeltaTime() >= 1.0f / FRAME_RATE) {
				Update();
				LateUpdate();
				Render();
				mTimer->Reset();
			}
		}
	}

	void GameManager::Update() {
		mInputManager->Update();

		mScreenManager->Update();
	}

	void GameManager::LateUpdate() {
		mInputManager->UpdatePrevInput();
		mPhysicsManager->Update();
		mCamera->Update();
	}

	void GameManager::Render() {
		mGraphics->ClearBackBuffer();
		mScreenManager->Render();
		mGraphics->Render();
	}

	GameManager::GameManager() : mQuit(false), mEvent(SDL_Event()) {
		Graphics::SetMode(Graphics::RenderMode::GL);
		mGraphics = Graphics::Instance();

		if (!Graphics::Initialized()) {
			mQuit = true;
		}

		mCamera = Camera::Instance();
		mAssetManager = AssetManager::Instance();
		mInputManager = InputManager::Instance();
		mAudioManager = AudioManager::Instance();
		mTimer = Timer::Instance();
		mScreenManager = ScreenManager::Instance();
		mPhysicsManager = PhysicsManager::Instance();
		mPhysicsManager->SetLayerCollisionMask(PhysicsManager::CollisionLayers::Friendly, PhysicsManager::CollisionFlags::Hostile | PhysicsManager::CollisionFlags::HostileProjectiles | PhysicsManager::CollisionFlags::Platforms);
		mPhysicsManager->SetLayerCollisionMask(PhysicsManager::CollisionLayers::FriendlyProjectiles, PhysicsManager::CollisionFlags::Hostile);
		mPhysicsManager->SetLayerCollisionMask(PhysicsManager::CollisionLayers::Hostile, PhysicsManager::CollisionFlags::Friendly | PhysicsManager::CollisionFlags::FriendlyProjectiles);
		mPhysicsManager->SetLayerCollisionMask(PhysicsManager::CollisionLayers::HostileProjectiles, PhysicsManager::CollisionFlags::Friendly);
		mPhysicsManager->SetLayerCollisionMask(PhysicsManager::CollisionLayers::Platforms, PhysicsManager::CollisionFlags::Friendly | PhysicsManager::CollisionFlags::FriendlyProjectiles | PhysicsManager::CollisionFlags::HostileProjectiles);
	}

	GameManager::~GameManager() {
		ScreenManager::Release();
		mScreenManager = nullptr;

		Timer::Release();
		mTimer = nullptr;

		AudioManager::Release();
		mAudioManager = nullptr;

		InputManager::Release();
		mInputManager = nullptr;

		AssetManager::Release();
		mAssetManager = nullptr;

		PhysicsManager::Release();
		mPhysicsManager = nullptr;

		Graphics::Release();
		mGraphics = nullptr;

		PlatformManager::Release();
		mPlatformManager = nullptr;

		Camera::Release();
		mCamera = nullptr;

		// Quit SDL subsystems
		SDL_Quit();
	}
}