
INL mat2 operator+(const mat2& m1, const mat2& m2) {
	mat2 mr;
	mr._m[0][0] = m1._m[0][0] + m2._m[0][0];
	mr._m[1][0] = m1._m[1][0] + m2._m[1][0];
	mr._m[0][1] = m1._m[0][1] + m2._m[0][1];
	mr._m[1][1] = m1._m[1][1] + m2._m[1][1];
	return mr;
}

INL mat2 operator*(const mat2& m1, const mat2& m2) {
	mat2 mr;
	mr._m[0][0] = m1._m[0][0] * m2._m[0][0] + m1._m[0][1] * m2._m[1][0];
	mr._m[1][0] = m1._m[1][0] * m2._m[0][0] + m1._m[1][1] * m2._m[1][0];
	mr._m[0][1] = m1._m[0][0] * m2._m[0][1] + m1._m[0][1] * m2._m[1][1];
	mr._m[1][1] = m1._m[1][0] * m2._m[0][1] + m1._m[1][1] * m2._m[1][1];
	return mr;
}

INL mat3 operator*(const mat3& m1, const mat3& m2) {
	mat3 mr;
	mr.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0];
	mr.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0];
	mr.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0];
	mr.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1];
	mr.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1];
	mr.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1];
	mr.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2];
	mr.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2];
	mr.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2];
	return mr;
}

// (2nd way only)
INL mat4 operator*(const mat4& m1, const mat4& m2) {
	mat4 mr;
	// 1st row
	//mr._m[0][0] = m1._m[0][0] * m2._m[0][0] + m1._m[1][0] * m2._m[0][1] + m1._m[2][0] * m2._m[0][2] + m1._m[3][0] * m2._m[0][3];//ok
	//mr._m[0][1] = m1._m[0][1] * m2._m[0][0] + m1._m[1][1] * m2._m[0][1] + m1._m[2][1] * m2._m[0][2] + m1._m[3][1] * m2._m[0][3];//ok
	//mr._m[0][2] = m1._m[0][2] * m2._m[0][0] + m1._m[1][2] * m2._m[0][1] + m1._m[2][2] * m2._m[0][2] + m1._m[3][2] * m2._m[0][3];//ok
	//mr._m[0][3] = m1._m[0][3] * m2._m[0][0] + m1._m[1][3] * m2._m[0][1] + m1._m[2][3] * m2._m[0][2] + m1._m[3][3] * m2._m[0][3];//ok
	// 2nd row (need 4)
	//mr._m[1][0] = ;

	// multiply for not transposed matrix
	//mr.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	//mr.m[0][1] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	//mr.m[0][2] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	//mr.m[0][3] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	// 
	//mr.m[1][0] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	//mr.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	//mr.m[1][2] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	//mr.m[1][3] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	// 
	//mr.m[2][0] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	//mr.m[2][1] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	//mr.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	//mr.m[2][3] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	// 
	//mr.m[3][0] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	//mr.m[3][1] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	//mr.m[3][2] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	//mr.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];

	// multiply for not transposed matrix
	//for (unsigned int j = 0; j < 4; j++)
	//	for (unsigned int i = 0; i < 4; i++)
	//		mr.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];

	for (unsigned int j = 0; j < 4; j++)
		for (unsigned int i = 0; i < 4; i++)
			mr.m[j][i] = m1.m[0][i] * m2.m[j][0] + m1.m[1][i] * m2.m[j][1] + m1.m[2][i] * m2.m[j][2] + m1.m[3][i] * m2.m[j][3];

	// 2nd way (easier)
	//vec4 v0 = m1 * vec4(m2._m[0][0], m2._m[0][1], m2._m[0][2], m2._m[0][3]);
	//vec4 v1 = m1 * vec4(m2._m[1][0], m2._m[1][1], m2._m[1][2], m2._m[1][3]);
	//vec4 v2 = m1 * vec4(m2._m[2][0], m2._m[2][1], m2._m[2][2], m2._m[2][3]);
	//vec4 v3 = m1 * vec4(m2._m[3][0], m2._m[3][1], m2._m[3][2], m2._m[3][3]);
	//mr._m[0][0] = v0.x; mr._m[0][1] = v0.y; mr._m[0][2] = v0.z; mr._m[0][3] = v0.w;
	//mr._m[1][0] = v1.x; mr._m[1][1] = v1.y; mr._m[1][2] = v1.z; mr._m[1][3] = v1.w;
	//mr._m[2][0] = v2.x; mr._m[2][1] = v2.y; mr._m[2][2] = v2.z; mr._m[2][3] = v2.w;
	//mr._m[3][0] = v3.x; mr._m[3][1] = v3.y; mr._m[3][2] = v3.z; mr._m[3][3] = v3.w;

	return mr;
}

INL mat4 translate(const mat4& m, const vec3& v) {
	mat4 mt(1);
	mt.m[3][0] = v.x;
	mt.m[3][1] = v.y;
	mt.m[3][2] = v.z;
	return m * mt;
}

// вроде ок (не знаю точной формулы)
INL mat4 scale(const mat4& m, const vec3& v) {
	mat4 ms(1);
	//ms = m;
	//ms._m[0][0] = m._m[0][0] * v.x;
	//ms._m[1][1] = m._m[1][1] * v.y;
	//ms._m[2][2] = m._m[2][2] * v.z;
	//return ms;

	ms.m[0][0] = v.x;
	ms.m[1][1] = v.y;
	ms.m[2][2] = v.z;
	return m * ms;
}

INL mat4 rotate(const mat4& m, float angle, const vec3& axis) {
	const float s = sin(angle);
	const float c = cos(angle);
	vec3 axs = axis;
	axs.normalize();
	const float minus1c = (1 - c);

	mat4 mr(1);
	mr = m;
	mr.m[0][0] = c + axs.x * axs.x * minus1c;
	mr.m[0][1] = axs.y * axs.x * minus1c + axs.z * s;
	mr.m[0][2] = axs.z * axs.x * minus1c - axs.y * s;

	mr.m[1][0] = axs.x * axs.y * minus1c - axs.z * s;
	mr.m[1][1] = c + axs.y * axs.y * minus1c;
	mr.m[1][2] = axs.z * axs.y * minus1c + axs.x * s;

	mr.m[2][0] = axs.x * axs.z * minus1c + axs.y * s;
	mr.m[2][1] = axs.y * axs.z * minus1c - axs.x * s;
	mr.m[2][2] = c + axs.z * axs.z * minus1c;

	//return m * mr;
	return mr;
}

INL mat4 rotateX(float angle) {
	const float c = cos(angle);
	const float s = sin(angle);
	mat4 mx(1, 0, 0, 0,
			0, c, -s, 0,
			0, s, c, 0,
			0, 0, 0, 1);
	return mx;
}

INL mat4 rotateY(float angle) {
	const float c = cos(angle);
	const float s = sin(angle);
	mat4 my(c, 0, -s, 0,
			0, 1, 0, 0,
			s, 0, c, 0,
			0, 0, 0, 1);
	return my;
}

INL mat4 rotateZ(float angle) {
	const float c = cos(angle);
	const float s = sin(angle);
	mat4 mz(c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	return mz;
}

INL mat4 perspectiveLH_OGLDEV(float fovy, float aspect, float zNear, float zFar) {
	const float DegToRad = acos(-1.0f) / 180;
	const float tanHalfFovy = tan(fovy / 2.0f * DegToRad); // convert to radians
	const float zRange = zFar - zNear;

	mat4 mProj;
	mProj.m[0][0] = 1.0f / tanHalfFovy;
	mProj.m[1][1] = 1.0f / (aspect * tanHalfFovy);
	mProj.m[2][2] = (zFar + zNear) / zRange;
	mProj.m[2][3] = 1;
	mProj.m[3][2] = -(2 * zFar * zNear) / zRange;
	return mProj;
}

INL mat4 perspectiveLH(float fovy, float aspect, float zNear, float zFar) {
	const float DegToRad = acos(-1.0f) / 180;
	const float tanHalfFovy = tan(fovy / 2.0f * DegToRad); // convert to radians
	const float zRange = zFar - zNear;

	mat4 mProj;
	mProj.m[0][0] = 1.0f / (aspect * tanHalfFovy);
	mProj.m[1][1] = 1.0f / tanHalfFovy;
	mProj.m[2][2] = (zFar + zNear) / zRange;
	mProj.m[2][3] = 1;
	mProj.m[3][2] = -(2 * zFar * zNear) / zRange;
	return mProj;
}

INL mat4 perspectiveLH_ZO(float fovy, float aspect, float zNear, float zFar) {
	const float DegToRad = acos(-1.0f) / 180;
	const float tanHalfFovy = tan(fovy / 2.0f * DegToRad); // convert to radians
	const float zRange = zFar - zNear;
	mat4 mProj;
	mProj.m[0][0] = 1.0f / (aspect * tanHalfFovy);
	mProj.m[1][1] = 1.0f / tanHalfFovy;
	mProj.m[2][2] = zFar / zRange;
	mProj.m[2][3] = 1;
	mProj.m[3][2] = -(zFar * zNear) / zRange;
	return mProj;
}

INL mat4 perspectiveRH(float fovy, float aspect, float zNear, float zFar) {
	const float DegToRad = acos(-1.0f) / 180;
	const float tanHalfFovy = tan(fovy / 2.0f * DegToRad); // convert to radians
	//const float tanHalfFovy = tan(fovy / 2.0f); // no convert to radians
	const float zRange = zFar - zNear;

	mat4 mProj;
	mProj.m[0][0] = 1.0f / (aspect * tanHalfFovy);
	mProj.m[1][1] = 1.0f / tanHalfFovy;
	mProj.m[2][2] = -(zFar + zNear) / zRange;
	mProj.m[2][3] = -1;
	mProj.m[3][2] = -(2 * zFar * zNear) / zRange;
	return mProj;
}

INL mat4 lookAtLH(const vec3& eye, const vec3& at, const vec3& up) {
	const vec3 f = normalize(at - eye);
	const vec3 r = normalize(crossProd(up, f));
	const vec3 u = crossProd(f, r);
	mat4 m(1);
	m.m[0][0] = r.x; m.m[0][1] = u.x; m.m[0][2] = f.x;
	m.m[1][0] = r.y; m.m[1][1] = u.y; m.m[1][2] = f.y;
	m.m[2][0] = r.z; m.m[2][1] = u.z; m.m[2][2] = f.z;
	m.m[3][0] = -dotProd(r, eye);
	m.m[3][1] = -dotProd(u, eye);
	m.m[3][2] = -dotProd(f, eye);
	return m;
}

INL mat4 lookAtRH(const vec3& eye, const vec3& at, const vec3& up) {
	const vec3 f = normalize(at - eye);
	const vec3 r = normalize(crossProd(f, up));
	const vec3 u = crossProd(r, f);
	mat4 m(r.x, u.x, -f.x, 0,
		   r.y, u.y, -f.y, 0,
		   r.z, u.z, -f.z, 0,
		-dotProd(r, eye), -dotProd(u, eye), dotProd(f, eye), 1);
	return m;
}

INL quat operator*(const my::quat& q1, const my::quat& q2) {
	return quat(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
				q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
				q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
				q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
}

INL quat operator*(const my::quat& q, const my::vec3& v) {
	return quat(-q.x * v.x - q.y * v.y - q.z * v.z,
				 q.w * v.x + q.y * v.z - q.z * v.y,
				 q.w * v.y + q.z * v.x - q.x * v.z,
				 q.w * v.z + q.x * v.y - q.y * v.x);
}
