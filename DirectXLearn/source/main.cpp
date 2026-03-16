#include "pch.h"
#include "MyEngineHeader.h"

class SandboxApp : public Application {
public:
	SandboxApp() {

	}

	~SandboxApp() {

	}

private:


protected:

	void start() override {
		createModel("TestImg");
	}

	void update(float dt) override {
		Application::update(dt);
	}
};

Application* createApplication() {
	return new SandboxApp();
}