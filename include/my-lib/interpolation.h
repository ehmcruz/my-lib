#ifndef __MY_LIB_INTERPOLATION_HEADER_H__
#define __MY_LIB_INTERPOLATION_HEADER_H__

#include <my-lib/std.h>


namespace Mylib
{

// ---------------------------------------------------

/*
	Tx is the type of the x-axis.
	For instance, can represent the time required for the interpolation.
*/

template <typename Tx>
class Interpolator
{
private:
	Tx x; // Current x-axis value. Varies from 0 to max_x.
	Tx max_x;

public:
	Interpolator (const Tx max_x_)
		: x(0), max_x(max_x_)
	{
	}

	Interpolator (const Tx max_x_)
		: max_x(max_x_)
	{
	}

	virtual ~Interpolator () = default;

	// Returns true if the interpolation is not finished.
	// Returns false otherwise.

	bool interpolate (const Tx delta_x)
	{
		this->x += delta_x;

		if (this->x > this->max_x) [[unlikely]]
			this->x = this->max_x;
		
		this->update__(this->x);

		return (this->x < this->max_x);
	}

protected:
	virtual void interpolate__ (const Tx x) = 0;
};

// ---------------------------------------------------

template <typename Tx, typename Ty>
class Interpolator__ : public Interpolator<Tx>
{
protected:
	Ty& target;

public:
	Interpolator__ (const Tx max_x_, Ty *target_)
		: Interpolator<Tx>(max_x_),
		target(*target_)
	{
		this->target = start_y_;
	}
};

// ---------------------------------------------------

template <typename Tx, typename Ty>
class LinearInterpolator : public Interpolator__<Tx, Ty>
{
protected:
	Ty start_y;
	Ty rate;

public:
	LinearInterpolator (const Tx max_x_, Ty *target_, Ty start_y_, Ty end_y_)
		: Interpolator__<Tx>(max_x_, target_),
		  start_y(start_y_),
		  rate((end_y_ - start_y_) / max_x_)
	{
	}

protected:
	void interpolate__ (const Tx x) override final
	{
		this->target = this->start_y + x * this->rate;
	}
};

// ---------------------------------------------------

} // end namespace Mylib

#endif