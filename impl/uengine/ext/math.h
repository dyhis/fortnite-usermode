#pragma once

namespace math
{
	class fvector
	{
	public:
		fvector() : x(), y(), z() {}
		fvector(double x, double y, double z) : x(x), y(y), z(z) {}

		fvector operator + (const fvector& other) const { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
		fvector operator - (const fvector& other) const { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
		fvector operator * (double offset) const { return { this->x * offset, this->y * offset, this->z * offset }; }
		fvector operator / (double offset) const { return { this->x / offset, this->y / offset, this->z / offset }; }

		fvector& operator *= (const double other) { this->x *= other; this->y *= other; this->z *= other; return *this; }
		fvector& operator /= (const double other) { this->x /= other; this->y /= other; this->z /= other; return *this; }

		fvector& operator = (const fvector& other) { this->x = other.x; this->y = other.y; this->z = other.z; return *this; }
		fvector& operator += (const fvector& other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
		fvector& operator -= (const fvector& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
		fvector& operator *= (const fvector& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
		fvector& operator /= (const fvector& other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

		operator bool() { return bool(this->x || this->y || this->z); }
		friend bool operator == (const fvector& a, const fvector& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
		friend bool operator != (const fvector& a, const fvector& b) { return !(a == b); }

		[[nodiscard]] inline double vector_scalar(const fvector& v) { return x * v.x + y * v.y + z * v.z; }
		[[nodiscard]] inline float distance_to(fvector v) { return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0))) / 100; }
		[[nodiscard]] inline double distance(fvector v) { return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0))); }
		[[nodiscard]] bool is_valid() const { return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z); }
		constexpr void in_validate() { this->x = this->y = this->z = std::numeric_limits<float>::infinity(); }
		[[nodiscard]] inline double length() { return sqrt(x * x + y * y + z * z); }

		void add_scale(const fvector& v, float scale)
		{
			x += v.x * scale;
			y += v.y * scale;
			z += v.z * scale;
		}


		__forceinline float size_squared() const { return x * x + y * y + z * z; }

		[[nodiscard]] inline double dot(const fvector& other) const
		{
			return (x * other.x) + (y * other.y) + (z * other.z);
		}

		double x, y, z;
	};
	class frotator
	{
	public:
		frotator() : pitch(0), yaw(0), roll(0) {}

		frotator(double Pitch, double Yaw, double Roll) : pitch(Pitch), yaw(Yaw), roll(Roll) {}

		frotator operator + (const frotator& other) const { return { this->pitch + other.pitch, this->yaw + other.yaw, this->roll + other.roll }; }
		frotator operator - (const frotator& other) const { return { this->pitch - other.pitch, this->yaw - other.yaw, this->roll - other.roll }; }
		frotator operator * (double offset) const { return { this->pitch * offset, this->yaw * offset, this->roll * offset }; }
		frotator operator / (double offset) const { return { this->pitch / offset, this->yaw / offset, this->roll / offset }; }

		frotator& operator = (const double other) { this->pitch = other; this->yaw = other; this->roll = other; return *this; }
		frotator& operator *= (const double other) { this->pitch *= other; this->yaw *= other; this->roll *= other; return *this; }
		frotator& operator /= (const double other) { this->pitch /= other; this->yaw /= other; this->roll /= other; return *this; }

		frotator& operator = (const frotator& other) { this->pitch = other.pitch; this->yaw = other.yaw; this->roll = other.roll; return *this; }
		frotator& operator += (const frotator& other) { this->pitch += other.pitch; this->yaw += other.yaw; this->roll += other.roll; return *this; }
		frotator& operator -= (const frotator& other) { this->pitch -= other.pitch; this->yaw -= other.yaw; this->roll -= other.roll; return *this; }
		frotator& operator /= (const frotator& other) { this->pitch /= other.pitch; this->yaw /= other.yaw; this->roll /= other.roll; return *this; }

		operator bool() const { return this->pitch != 0 && this->yaw != 0 && this->roll != 0; }

		friend bool operator == (const frotator& a, const frotator& b) { return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll; }
		friend bool operator != (const frotator& a, const frotator& b) { return !(a == b); }

		frotator get() const
		{
			return frotator(pitch, yaw, roll);
		}

		void set(double _Pitch, double _Yaw, double _Roll)
		{
			pitch = _Pitch;
			yaw = _Yaw;
			roll = _Roll;
		}

		double length() const
		{
			return std::sqrt(pitch * pitch + yaw * yaw + roll * roll);
		}

		double dot(const frotator& V) const
		{
			return pitch * V.pitch + yaw * V.yaw + roll * V.roll;
		}

		double distance(const frotator& V) const
		{
			return std::sqrt(std::pow(V.pitch - this->pitch, 2.0) + std::pow(V.yaw - this->yaw, 2.0) + std::pow(V.roll - this->roll, 2.0));
		}



		math::fvector to_vector()
		{
			return math::fvector(pitch, yaw, roll);
		}

		static void normalize_rotation(frotator& in)
		{
			in.pitch = fmod(in.pitch + 180.0, 360.0) - 180.0;
			in.yaw = fmod(in.yaw + 180.0, 360.0) - 180.0;
			in.roll = fmod(in.roll + 180.0, 360.0) - 180.0;
		}
		double pitch;
		double yaw;
		double roll;
	};

	static fvector find_rotation(const fvector& start, const fvector& target)
	{
		math::fvector dir = (target - start) / (target - start).length();
		return { std::atan2(dir.z, sqrt(dir.x * dir.x + dir.y * dir.y)) * (180.0 / std::numbers::pi), atan2(dir.y, dir.x) * (180.0 / std::numbers::pi), 0.0 };
	}

	static double clamp_angle(double value, double min, double max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}

	static fvector clamp_rotation(const fvector& start_rotation, const fvector& end_rotation, float smooth_scale_x, float smooth_scale_y)
	{
		fvector delta = end_rotation - start_rotation;

		delta.y = fmod(delta.y + 360.0, 360.0);
		if (delta.y > 180.0) delta.y -= 360.0;

		delta.x = clamp_angle(delta.x, -74.0, 74.0);
		return { delta.x / smooth_scale_x + start_rotation.x,delta.y / smooth_scale_y + start_rotation.y,0.0 };
	}

	static void normalize_rotation(fvector& in)
	{
		in.x = fmod(in.x + 180.0, 360.0) - 180.0;
		in.y = fmod(in.y + 180.0, 360.0) - 180.0;
		in.z = fmod(in.z + 180.0, 360.0) - 180.0;
	}

	struct fvector2d
	{
		fvector2d() : x(), y() {}
		fvector2d(double x, double y) : x(x), y(y) {}

		fvector2d operator + (const fvector2d& other) const { return { this->x + other.x, this->y + other.y }; }
		fvector2d operator - (const fvector2d& other) const { return { this->x - other.x, this->y - other.y }; }
		fvector2d operator * (double offset) const { return { this->x * offset, this->y * offset }; }
		fvector2d operator / (double offset) const { return { this->x / offset, this->y / offset }; }

		fvector2d& operator *= (const double other) { this->x *= other; this->y *= other; return *this; }
		fvector2d& operator /= (const double other) { this->x /= other; this->y /= other; return *this; }

		fvector2d& operator = (const fvector2d& other) { this->x = other.x; this->y = other.y; return *this; }
		fvector2d& operator += (const fvector2d& other) { this->x += other.x; this->y += other.y; return *this; }
		fvector2d& operator -= (const fvector2d& other) { this->x -= other.x; this->y -= other.y; return *this; }
		fvector2d& operator *= (const fvector2d& other) { this->x *= other.x; this->y *= other.y; return *this; }
		fvector2d& operator /= (const fvector2d& other) { this->x /= other.x; this->y /= other.y; return *this; }

		operator bool() { return bool(this->x || this->y); }
		friend bool operator == (const fvector2d& A, const fvector2d& B) { return A.x == B.x && A.y == A.y; }
		friend bool operator != (const fvector2d& A, const fvector2d& B) { return !(A == B); }
		static fvector2d from_angle(double angle, double magnitude = 1.0f) { return fvector2d(std::cos(angle) * magnitude, std::sin(angle) * magnitude); }

		double vector_scalar(const fvector2d& V) { return x * V.x + y * V.y; }
		double distance(fvector2d V) { return double(sqrtf(powf(V.x - this->x, 2.0) + powf(V.y - this->y, 2.0))); }

		[[nodiscard]] bool is_valid() const { return std::isfinite(this->x) && std::isfinite(this->y); }

		double x, y;
	};

	struct fplane : fvector
	{
		double w = 0.0;
	};

	typedef struct _dbl_matrix
	{
		union
		{
			struct
			{
				double _11, _12, _13, _14;
				double _21, _22, _23, _24;
				double _31, _32, _33, _34;
				double _41, _42, _43, _44;
			};
			double m[4][4];
		};
	} dbl_matrix;

	struct alignas(16) fmatrix : public _dbl_matrix
	{
		fplane x_plane{};
		fplane y_plane{};
		fplane z_plane{};
		fplane w_plane{};

		constexpr fmatrix() = default;
	};

	inline fmatrix matrix_multiplication(const fmatrix& matrix1, const fmatrix& matrix2)
	{
		fmatrix result{};

		result._11 = matrix1._11 * matrix2._11 + matrix1._12 * matrix2._21 + matrix1._13 * matrix2._31 + matrix1._14 * matrix2._41;
		result._12 = matrix1._11 * matrix2._12 + matrix1._12 * matrix2._22 + matrix1._13 * matrix2._32 + matrix1._14 * matrix2._42;
		result._13 = matrix1._11 * matrix2._13 + matrix1._12 * matrix2._23 + matrix1._13 * matrix2._33 + matrix1._14 * matrix2._43;
		result._14 = matrix1._11 * matrix2._14 + matrix1._12 * matrix2._24 + matrix1._13 * matrix2._34 + matrix1._14 * matrix2._44;

		result._21 = matrix1._21 * matrix2._11 + matrix1._22 * matrix2._21 + matrix1._23 * matrix2._31 + matrix1._24 * matrix2._41;
		result._22 = matrix1._21 * matrix2._12 + matrix1._22 * matrix2._22 + matrix1._23 * matrix2._32 + matrix1._24 * matrix2._42;
		result._23 = matrix1._21 * matrix2._13 + matrix1._22 * matrix2._23 + matrix1._23 * matrix2._33 + matrix1._24 * matrix2._43;
		result._24 = matrix1._21 * matrix2._14 + matrix1._22 * matrix2._24 + matrix1._23 * matrix2._34 + matrix1._24 * matrix2._44;

		result._31 = matrix1._31 * matrix2._11 + matrix1._32 * matrix2._21 + matrix1._33 * matrix2._31 + matrix1._34 * matrix2._41;
		result._32 = matrix1._31 * matrix2._12 + matrix1._32 * matrix2._22 + matrix1._33 * matrix2._32 + matrix1._34 * matrix2._42;
		result._33 = matrix1._31 * matrix2._13 + matrix1._32 * matrix2._23 + matrix1._33 * matrix2._33 + matrix1._34 * matrix2._43;
		result._34 = matrix1._31 * matrix2._14 + matrix1._32 * matrix2._24 + matrix1._33 * matrix2._34 + matrix1._34 * matrix2._44;

		result._41 = matrix1._41 * matrix2._11 + matrix1._42 * matrix2._21 + matrix1._43 * matrix2._31 + matrix1._44 * matrix2._41;
		result._42 = matrix1._41 * matrix2._12 + matrix1._42 * matrix2._22 + matrix1._43 * matrix2._32 + matrix1._44 * matrix2._42;
		result._43 = matrix1._41 * matrix2._13 + matrix1._42 * matrix2._23 + matrix1._43 * matrix2._33 + matrix1._44 * matrix2._43;
		result._44 = matrix1._41 * matrix2._14 + matrix1._42 * matrix2._24 + matrix1._43 * matrix2._34 + matrix1._44 * matrix2._44;

		return result;
	}

	inline fmatrix create_rotation_matrix(const fvector& rotation)
	{
		auto rad_x = rotation.x * double(std::numbers::pi) / 180.f;
		auto rad_y = rotation.y * double(std::numbers::pi) / 180.f;
		auto rad_z = rotation.z * double(std::numbers::pi) / 180.f;

		auto sp = std::sinf(rad_x), cp = std::cosf(rad_x);
		auto sy = std::sinf(rad_y), cy = std::cosf(rad_y);
		auto sr = std::sinf(rad_z), cr = std::cosf(rad_z);

		fmatrix matrix{};
		matrix.m[0][0] = cp * cy;
		matrix.m[0][1] = cp * sy;
		matrix.m[0][2] = sp;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = sr * sp * cy - cr * sy;
		matrix.m[1][1] = sr * sp * sy + cr * cy;
		matrix.m[1][2] = -sr * cp;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(cr * sp * cy + sr * sy);
		matrix.m[2][1] = cy * sr - cr * sp * sy;
		matrix.m[2][2] = cr * cp;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = matrix.m[3][1] = matrix.m[3][2] = 0.0f;
		matrix.m[3][3] = 1.0f;

		return matrix;
	}
	struct ftransform
	{
		fplane rot;
		fvector translation;
		char pad_38[4];
		fvector scale;
		char pad_58[4];

		inline fmatrix to_matrix() const
		{
			fmatrix m{};

			m._41 = translation.x;
			m._42 = translation.y;
			m._43 = translation.z;

			auto x2 = rot.x + rot.x, y2 = rot.y + rot.y, z2 = rot.z + rot.z;
			auto xx2 = rot.x * x2, yy2 = rot.y * y2, zz2 = rot.z * z2;
			m._11 = (1.0f - (yy2 + zz2)) * 1.0;
			m._22 = (1.0f - (xx2 + zz2)) * 1.0;
			m._33 = (1.0f - (xx2 + yy2)) * 1.0;

			auto yz2 = rot.y * z2, wx2 = rot.w * x2;
			m._32 = (yz2 - wx2) * 1.0;
			m._23 = (yz2 + wx2) * 1.0;

			auto xy2 = rot.x * y2, wz2 = rot.w * z2;
			m._21 = (xy2 - wz2) * 1.0;
			m._12 = (xy2 + wz2) * 1.0;

			auto xz2 = rot.x * z2, wy2 = rot.w * y2;
			m._31 = (xz2 + wy2) * 1.0;
			m._13 = (xz2 - wy2) * 1.0;

			m._14 = m._24 = m._34 = 0.0f;
			m._44 = 1.0f;

			return m;
		}
	};
}
