// DO NOT EDIT THIS FILE
// generated using scripts/generate.hy and code in src/dsl/

#pragma once
#define PINEAL(_)

namespace primitives{
	PINEAL("cube")
	void cube(double r){
		ofDrawBox(r);
	}

	PINEAL("polygon")
	void polygon_n_r(int n, float r){
		static unordered_map<int, shared_ptr<ofPolyline>> polygons;
		shared_ptr<ofPolyline> p;

		if(polygons.find(n) == polygons.end()){
			p = make_shared<ofPolyline>();

			float angle, x, y;

			for(int i = 0; i < n; i++){
				angle = PI / 2 + i * TWO_PI / n;
				x = cos(angle);
				y = sin(angle);
				p->addVertex(ofPoint(x,y));
			}
			p->close();

			polygons[n] = p;
		}else{
			p = polygons[n];
		}
		ofPushMatrix();
		ofScale(r, r, r);
		p->draw();
		ofPopMatrix();
	}

	PINEAL("polygon")
	void polygon_n(int n){
		polygon_n_r(n, 1);
	}
}

namespace layers{
	unordered_map<string, shared_ptr<ofFbo>> layers_map;

	void new_layer(string name){
		if(layers_map.find(name) != layers_map.end()){
			return;
		}
		auto fbo = make_shared<ofFbo>();

		fbo->allocate(BUFFER_SIZE, BUFFER_SIZE, GL_RGBA);
		fbo->begin();
		ofClear(255,255,255, 0);
		fbo->end();
		layers_map[name] = fbo;
	}

	PINEAL("on_layer")
	void on_layer(py::object f, string name){
		if(layers_map.find(name) == layers_map.end()){
			new_layer(name);
		}
		ofEasyCam camera;
		camera.setDistance(1);
		camera.setNearClip(0.01);

		layers_map[name]->begin();
		camera.begin();
		f();
		camera.end();
		layers_map[name]->end();
	}

	PINEAL("draw_layer")
	void draw_layer(string name){
		if(layers_map.find(name) == layers_map.end()){
			new_layer(name);
		}
		layers_map[name]->getTexture().draw(-1, -1, 2, 2);
	}
}

namespace transformations{
	PINEAL("scale")
	void scale_xyz(py::object f, double x, double y, double z){
		ofPushMatrix();
		ofScale(x, y, z);
		f();
		ofPopMatrix();
	}

	PINEAL("scale")
	void scale_xy(py::object f, double x, double y){
		scale_xyz(f, x, y, 1);
	}

	PINEAL("scale")
	void scale_r(py::object f, double r){
		scale_xyz(f, r, r, r);
	}

	PINEAL("translate")
	void translate_xyz(py::object f, double x, double y, double z){
		ofPushMatrix();
		ofTranslate(x, y, z);
		f();
		ofPopMatrix();
	}

	PINEAL("translate")
	void translate_xy(py::object f, double x, double y){
		translate_xyz(f, x, y, 0);
	}

	PINEAL("translate")
	void translate_x(py::object f, double x){
		translate_xyz(f, x, 0, 0);
	}

	PINEAL("rotate_x")
	void rotate_x(py::object f, double rad){
		ofPushMatrix();
		ofRotateX(180 * rad / PI);
		f();
		ofPopMatrix();
	}

	PINEAL("rotate_y")
	void rotate_y(py::object f, double rad){
		ofPushMatrix();
		ofRotateY(180 * rad / PI);
		f();
		ofPopMatrix();
	}

	PINEAL("rotate_z")
	void rotate_z(py::object f, double rad){
		ofPushMatrix();
		ofRotateZ(180 * rad / PI);
		f();
		ofPopMatrix();
	}

	PINEAL("turn_x")
	void turn_x(py::object f, int n){
		ofPushMatrix();
		for(int i=0; i<n; i++){
			f();
			ofRotateX(360.0 / n);
		}
		ofPopMatrix();
	}

	PINEAL("turn_y")
	void turn_y(py::object f, int n){
		ofPushMatrix();
		for(int i=0; i<n; i++){
			f();
			ofRotateY(360.0 / n);
		}
		ofPopMatrix();
	}

	PINEAL("turn_z")
	void turn_z(py::object f, int n){
		ofPushMatrix();
		for(int i=0; i<n; i++){
			f();
			ofRotateZ(360.0 / n);
		}
		ofPopMatrix();
	}
}

namespace colors{
	PINEAL("background")
	void background(double r, double g, double b, double a){
		ofBackground(r * 255, g * 255, b * 255, a * 255);
	}

	PINEAL("color")
	void color(py::object f, double r, double g, double b, double a){
		static ofColor status_color;
		ofColor old_color = status_color;
		ofColor new_color = ofColor(r * 255, g * 255, b * 255, a * 255);

		status_color = new_color;
		ofSetColor(status_color);

		f();

		status_color = old_color;
		ofSetColor(status_color);
	}

	void fill_status(py::object f, bool status){
		static bool status_fill = true;
		bool old_fill = status_fill;
		bool new_fill = status;

		status_fill = new_fill;
		if(status_fill){
			ofFill();
		}else{
			ofNoFill();
		}

		f();

		status_fill = old_fill;
		if(status_fill){
			ofFill();
		}else{
			ofNoFill();
		}
	}

	PINEAL("fill")
	void fill(py::object f){
		fill_status(f, true);
	}

	PINEAL("no_fill")
	void no_fill(py::object f){
		fill_status(f, false);
	}

	PINEAL("line_width")
	void line_width(py::object f, double new_width){
		static double status_line_width = 1;
		double old_width = status_line_width;

		status_line_width = new_width;
		ofSetLineWidth(status_line_width);

		f();

		status_line_width = old_width;
		ofSetLineWidth(status_line_width);
	}
}

BOOST_PYTHON_MODULE(core){
	py::def("cube", &primitives::cube);
	py::def("polygon", &primitives::polygon_n_r);
	py::def("polygon", &primitives::polygon_n);

	py::def("on_layer", &layers::on_layer);
	py::def("draw_layer", &layers::draw_layer);

	py::def("scale", &transformations::scale_xyz);
	py::def("scale", &transformations::scale_xy);
	py::def("scale", &transformations::scale_r);
	py::def("translate", &transformations::translate_xyz);
	py::def("translate", &transformations::translate_xy);
	py::def("translate", &transformations::translate_x);
	py::def("rotate_x", &transformations::rotate_x);
	py::def("rotate_y", &transformations::rotate_y);
	py::def("rotate_z", &transformations::rotate_z);
	py::def("turn_x", &transformations::turn_x);
	py::def("turn_y", &transformations::turn_y);
	py::def("turn_z", &transformations::turn_z);

	py::def("background", &colors::background);
	py::def("color", &colors::color);
	py::def("fill", &colors::fill);
	py::def("no_fill", &colors::no_fill);
	py::def("line_width", &colors::line_width);
}