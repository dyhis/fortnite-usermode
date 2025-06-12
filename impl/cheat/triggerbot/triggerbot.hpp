class triggerbot
{
public:
	void go();
private:
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	std::chrono::steady_clock::time_point shoot_start_time;

	bool has_clicked;
	bool ready_to_shoot;
}inline c_triggerbot;