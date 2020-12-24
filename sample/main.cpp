#include "base_utils.h"
#include "simple_objects.h"
#include "compound_obj.h"
#include "complex_obj.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

/*Controllers

Python Object Controller:
Interface for python to direct and obtain statistics on objects in the world (create object, find position)

Python World Controller:
Interface for python to direct and obtain statistics on objects on the world (create wind, determine "weather")

Logic Controller:
Controls all objects > has them in objects
Move objects
Calculate any logic for world

*/

/* Classes
* World Object
* Static Controller Object
* 
* Simple Objects
* Compound Object
* Complex(logic) Object
* Camera Object
*/


//Box and Terrain Test
void test1() {
	key_controller* controller = new key_controller{};
	world* main_world = new world{ controller };
	new terrain_obj(main_world);

	while (main_world->device->run() && main_world->driver)
	{
		main_world->update();

		main_world->framerate();

		if (controller->IsKeyDown(KEY_KEY_S))
		{
			new box_obj{ main_world,new int[] {10,10,10},new int[] {1,1,10},1 };
			controller->clear(KEY_KEY_S);
		}
		if (controller->IsKeyDown(KEY_ESCAPE))
		{
			break;
		}

	}
	delete main_world;
	delete controller;
	
}

//Wheel and Constraint Test
void test2() 
{
	key_controller* controller = new key_controller{};
	world* main_world = new world{ controller };
	main_obj* hg = new compound_obj{ main_world };
	terrain_obj* terr = new terrain_obj{ main_world };

	while (main_world->device->run() && main_world->driver)
	{
		main_world->update();
		main_world->framerate();
		if (controller->IsKeyDown(KEY_KEY_W))
		{
			if (hg != nullptr)
				hg->forward();

		}
		if (hg != nullptr && !controller->IsKeyDown(KEY_KEY_W))
		{
			hg->stop();
			controller->clear(KEY_KEY_W);
		}
	}
	delete terr;
	delete hg;
	delete main_world;
	delete controller;
}

//Camera Test
void test3() 
{
	key_controller* controller = new key_controller{};
	world* main_world = new world{ controller };
	terrain_obj* terr = new terrain_obj{main_world};

	box_obj *obj =  new box_obj{ main_world,new int[] {1,1,1},new int[] {1,1,1},1 };

	
	ICameraSceneNode* cameraNode;
	cameraNode = main_world->scenemgr->addCameraSceneNode(obj->irr_body, vector3df(0, 5, 10));
	cameraNode->setFOV(0.78f);
	//cameraNode->setPosition(vector3df(0, 0, 10));
	
	while (main_world->device->run() && main_world->driver)
	{
		obj->irr_body->updateAbsolutePosition();
		cameraNode->setTarget(obj->irr_body->getAbsolutePosition());

		main_world->update();
		main_world->framerate();

		if (controller->IsKeyDown(KEY_KEY_W))
		{
			obj->forward();
			controller->clear(KEY_KEY_W);
		}
		if (controller->IsKeyDown(KEY_KEY_S))
		{
			obj->stop();
			controller->clear(KEY_KEY_S);
		}
	}
	delete main_world;
	delete controller;
	
}

//Memory Leak Test
void test4()
{
	key_controller* controller = new key_controller{};
	world *main_world = new world{ controller };

	box_obj* box = nullptr;
	terrain_obj* terr = nullptr;
	main_obj* main = nullptr;

	ICameraSceneNode* cameraNode = main_world->scenemgr->addCameraSceneNode();

	while (main_world->device->run() && main_world->driver)
	{
		main_world->update();
		main_world->device->run(); 
		main_world->framerate();
		/*
		if (controller->IsKeyDown(KEY_KEY_W))
		{
			box = new box_obj{ main_world,new int[] {1,1,1},new int[] {1,1,1},1 };
			controller->clear(KEY_KEY_W);
		}

		if (controller->IsKeyDown(KEY_KEY_S))
		{
			if(box != nullptr)
				delete box;
			controller->clear(KEY_KEY_S);
		}
			
		if (controller->IsKeyDown(KEY_KEY_A))
		{
			terr = new terrain_obj{main_world};
			controller->clear(KEY_KEY_A);
		}

		if (controller->IsKeyDown(KEY_KEY_D))
		{
			if (terr != nullptr)
				delete terr;
			terr = nullptr;
			controller->clear(KEY_KEY_D);
		}
		*/
		if (controller->IsKeyDown(KEY_KEY_Q))
		{
			main = new compound_obj{ main_world };
			controller->clear(KEY_KEY_Q);
		}

		if (controller->IsKeyDown(KEY_KEY_E))
		{
			if (main != nullptr)
				delete main;
			main = nullptr;
			controller->clear(KEY_KEY_E);
		}
	}
	delete main_world;
	delete controller; 
}

//OBJ load test
void test5()
{
	key_controller* controller = new key_controller{};
	world* main_world = new world{ controller };
	//load_obj(main_world);
	load_obj(main_world, "../textures/city/obj2.dae");
	//create_node(main_world, new int[] { 3,3,3 });



	while (main_world->device->run() && main_world->driver)
	{
		main_world->update();
		main_world->device->run();
		main_world->framerate();
	}
}

//Logical Object Test
void test6()
{
	//Helicopter
	//Helicopter is lifted by force on rotor
	//Rotor force is relative to collective and engine rpm
	//engine rpm depends on environment(air density, load, etc)
	//update logic along with world

	key_controller* controller = new key_controller{};
	world* main_world = new world{ controller };
	new terrain_obj(main_world);

	complex_obj* heli = new complex_obj(main_world);

	ICameraSceneNode* cameraNode;

	cameraNode = main_world->scenemgr->addCameraSceneNode();
	heli->frame->irr_body->addChild(cameraNode);
	cameraNode->setFOV(0.78f);
	cameraNode->setPosition(vector3df(10, 5, 0));

	while (main_world->device->run() && main_world->driver)
	{
		cameraNode->setTarget(heli->frame->irr_body->getAbsolutePosition());
		main_world->update();
		main_world->framerate();
		heli->update_logic();
		if (controller->IsKeyDown(KEY_KEY_P))
		{
			heli->direct(complex_obj::direction::up);
		}
		if (controller->IsKeyDown(KEY_KEY_L))
		{
			heli->direct(complex_obj::direction::down);
		}
		if (controller->IsKeyDown(KEY_KEY_W))
		{
			heli->direct(complex_obj::direction::front);
		}
		if (controller->IsKeyDown(KEY_KEY_S))
		{
			heli->direct(complex_obj::direction::back);
		}
		if (controller->IsKeyDown(KEY_KEY_A))
		{
			heli->direct(complex_obj::direction::left);
		}
		if (controller->IsKeyDown(KEY_KEY_D))
		{
			heli->direct(complex_obj::direction::right);
		}
		if (controller->IsKeyDown(KEY_KEY_I))
		{
			system("dir");
			std::cout<< heli->frame->body->getWorldTransform().getRotation().getW() << std::endl;
			std::cout<< heli->frame->body->getWorldTransform().getRotation().getX() << std::endl;
			std::cout<< heli->frame->body->getWorldTransform().getRotation().getY() << std::endl;
			std::cout<< heli->frame->body->getWorldTransform().getRotation().getZ() << std::endl;
			std::cout << heli->frame->body->getWorldTransform().getOrigin().getX() << std::endl;
			std::cout << heli->frame->body->getWorldTransform().getOrigin().getY() << std::endl;
			std::cout << heli->frame->body->getWorldTransform().getOrigin().getZ() << std::endl;
			heli->getProperties();
		}

	}
	delete main_world;
	delete controller;
}

//Visualization tools
void test7()
{

}

//Python Paramter Import Test

#if defined _DEBUG
	int main()
	{
		int choice;
		do {
			printf("Choose test, 0 to exit: ");
			std::cin >> choice;
			switch (choice)
			{
			case 1:
				test1();
				break;
			case 2:
				test2();
				break;
			case 3:
				test3();
				break;
			case 4:
				test4();
				break;
			case 5:
				test5();
				break;
			case 6:
				test6();
				break;

			}
		} while (choice != 0);

		return 0;
	}


#else
	PYBIND11_MODULE(sample, m) 
	{
		// Same binding code
		py::class_<py_obj>(m, "py_obj");
		py::class_<complex_obj, py_obj>(m, "complex_obj")
			.def(py::init<pyWorld*>())
			.def("update_logic", &complex_obj::update_logic, R"pbdoc(Update the helicopter logic)pbdoc")
			.def("getProperties", &complex_obj::getProperties, R"pbdoc(Get the helicopter properties)pbdoc");
			//.def("remove_heli", &complex_obj::~complex_obj)
		// Again, return a base pointer to a derived instance
		m.def("create_heli", [](pyWorld* arg) { return std::unique_ptr<py_obj>(new complex_obj(arg)); });
		py::class_<pyWorld>(m, "pyWorld")
			.def(py::init<>())
			.def("update", &pyWorld::update);
		#ifdef VERSION_INFO
			m.attr("__version__") = VERSION_INFO;
		#else
			m.attr("__version__") = "dev";
		#endif
	}

#endif
