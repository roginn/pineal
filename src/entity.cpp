#include "pineal.h"

Entity::Entity(){
	draw = [](){};
}

Entity::Entity(py::object f){
	draw = [=](){ f(); };
}

Entity::Entity(function<void(void)> f){
	draw = f;
}

void Entity::__call__(){
	draw();
}

void Entity::operator()() const{
	draw();
}


unordered_map<string, Entity> layers_map;


Entity get_layer(string name){
	if(layers_map.find(name) == layers_map.end()){
		return Entity();
	}else{
		return layers_map[name];
	}
}


void set_layer(string name, Entity e){
	layers_map[name] = e;
}


unordered_map<string, ofTrueTypeFont> fonts_map;


Entity text(string font_name, string s){
	float size = 100;

	if(fonts_map.find(font_name) == fonts_map.end()){
		ofTrueTypeFont font;
		font.load(font_name, size, true, true, true);
		fonts_map[font_name] = font;
	}

	float scale = 1.0 / size;

	Entity e([=](){
		ofPushMatrix();
		ofScale(scale, scale, scale);
		fonts_map[font_name].drawStringAsShapes(s, 0, 0);
		ofPopMatrix();
	});

	return e;
}


Entity make_entity(string name, py::list args){

	if(name=="change"){
		Entity entity = py::extract<Entity>(args[0]);
		Action action = py::extract<Action>(args[1]);

		return action(entity);
	}

	if(name=="group"){
		size_t n = py::len(args);
		vector<Entity> entities;

		for(size_t i=0; i<n; i++){
			entities.push_back(py::extract<Entity>(args[i]));
		}
		Entity e([=](){
			for(size_t i=0; i<entities.size(); i++){
				entities[i]();
			}
		});

		return e;
	}

	if(name=="cube"){
		Entity e([=](){
			ofDrawBox(0.5);
		});

		return e;
	}

	if(name=="polygon"){
		int n = py::extract<int>(args[0]);

		Entity e([=](){
			ofPushMatrix();

			ofRotateZ(90);

			ofSetCircleResolution(n);
			ofDrawCircle(0, 0, 0.5);

			ofPopMatrix();
		});

		return e;
	}

	if(name=="text"){
		string font_name = py::extract<string>(args[0]);
		string s = py::extract<string>(args[1]);
		return text(font_name, s);
	}

	if(name=="osc_text"){
		string font_name = py::extract<string>(args[0]);
		string path = py::extract<string>(args[1]);
		Entity e([=](){
			string s = osc_get_string(path);
			text(font_name, s)();
		});
		return e;
	}

	if(name=="layer"){
		string layer_name = py::extract<string>(args[0]);
		Entity e([=](){
			get_layer(layer_name)();
		});
		return e;
	}

	return Entity();
}