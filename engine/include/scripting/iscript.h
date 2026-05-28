#pragma once

#include <concepts>
#include "gameobject/gameobject.h"

class Physicalbody;
class ScriptManager;

namespace Scripting
{
	class ENIGMA_API IScript : public IComponent
	{
	public:
		IScript() = default;
		~IScript() override = default;

		const char* GetScriptName() const { return scriptName; };

		void Destroy() final;
		virtual void Awake() {}
		virtual void Start() {}
		virtual void Update(float deltaTime) {}
		virtual void FixedUpdate(float fixedDeltaTime) {}
		virtual void LateUpdate(float deltaTime) {}
		virtual void End() {}

		virtual void OnCollisionEnter(Physicalbody& body1, Physicalbody& body2) {}
		virtual void OnCollisionStay(Physicalbody& body1, Physicalbody& body2) {}
		virtual void OnCollisionExit(Physicalbody& body1, Physicalbody& body2) {}

		template<typename T>
		requires std::derived_from<T, IComponent>
		T* GetComponent() const;

		template<typename T>
		requires std::derived_from<T, IComponent>
		T* AddComponent() const;

		bool IsEnabled() const { return enabled; }
		void SetEnabled(bool value) { enabled = value; }

	protected:
		const char* scriptName = "UnnamedScript";

	private:
		RTTR_ENABLE(IComponent);
		friend class ::ScriptManager;
		bool enabled = true;

		bool awake = true;
		bool start = true;
		bool update = true;
		bool fixedUpdate = true;
		bool lateUpdate = true;
		bool end = true;
		bool onCollisionEnter = true;
		bool onCollisionStay = true;
		bool onCollisionExit = true;
	};
}

#include "iscript.inl"