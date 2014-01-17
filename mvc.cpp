class Button; // Prewritten GUI element

class GraphGUI {
public:
	GraphGUI() {
		_button = new Button ("Click Me");
		_model = new GraphData();
		_controller = new GraphController (_model, _button);
	}
	~GraphGUI() {
		delete _button;
		delete _model;
		delete _controller;
	}

	drawGraph() {
		// Use model's data to draw the graph somehow
	}
	...

private:
	Button*			_button;
	GraphData*		_model;
	GraphController*	_controller;
};

class GraphData {
public:
	GraphData() {
		_number = 10;
	}
	void increaseNumber() {
		_number += 10;
	}
	const int getNumber() { return _number; }
private:
	int _number;
};

class GraphController {
public:
	GraphController (GraphData* model, Button* button) {
		__model = model;
		__button = button;
		__button->setClickHandler (this, &onButtonClicked);
	}

	void onButtonClicked() {
		__model->increaseNumber();
	}

private:
	// Don't handle memory
	GraphData*	__model;
	Button*		__button;
};
