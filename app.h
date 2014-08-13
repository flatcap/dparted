#ifndef _APP_H_
#define _APP_H_

class App
{
public:
	App (void);
	virtual ~App();

	void initialise (void);

protected:
	bool start_plugin (const std::string& filename);
};

#endif // _APP_H_

