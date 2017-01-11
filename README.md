# Roadmap

- Refactor Material to derive from Asset.
- Refactor ShaderProgram to derive from Asset.
- Refactor Mesh to derive from Asset.
- Refactor Light to derive from Asset.
- Refactor Boost.Filesystem away (and use a fileystem library on github).
- Refactor Boost.Serialization away (use cereal instead).
- Think about embedding Python or ChaiScript. Which one? Both? Others?
- Create a Transform class, derived from Component.
- Get ImGui to work nicely with the Renderer.
- Refactor the Renderer god-class.
- Supply a "main loop" from the library?
- Implement an #include statement for GLSL.
- Normalize #include paths in all headers.

<!-- \f$\newcommand{\AffMat}{\mathop{\rm AffMat}\nolimits}\f$
\f$\newcommand{\GL}{\mathop{\rm GL}\nolimits}\f$
\f$\newcommand{\Mat}{\mathop{\rm Mat}\nolimits}\f$
\f$\newcommand{\intercal}{T}\f$
\f$\newcommand{\SO}{\mathop{\rm SO}\nolimits}\f$
\f$\newcommand{\OrthogonalGroup}{\mathop{\rm O}\nolimits}\f$

# <a name="introduction"></a>Introduction

Gintonic is the name of an engine framework written in C++ using OpenGL.
The project is ongoing.

# <a name="dependencies"></a>Dependencies

Gintonic depends on the following software:

* CMake -- For building
* Boost libraries -- Cross-platform filesystem, serialization, iostreams, etc.
* Freetype -- For rendering fonts
* libjpeg -- If on OSX/Linux, for texture loading
* libpng -- If on OSX/Linux, for texture loading
* SDL2 -- For a cross-platform windowing system
* FBX SDK -- For importing meshes, materials, light, etc.

## <a name="installing-windows-dependencies"></a>Installing Windows Dependencies

You do not have to install libjpeg and libpng. Loading images into memory can 
be done with the WIC API of Windows. There is no universal way to install
dependencies on Windows, so it can be a bit of a pain to do this. CMake has an
installer, so no problem there. I recommend downloading the precompiled 
binaries of SDL2. The website of the Freetype project has some links to
websites which provide precompiled binaries. I suggest making a separate
folder for each dependency in your top-level directory. For instance,
C:\\Freetype, C:\\SDL2, and so forth. The FBX SDK has an installer, so that's
nice too. The FBX SDK will be installed in %programfiles%\\Autodesk\\FBX. Once
the dependencies are installed, you need to setup some environment variables.
The top-level CMakeLists.txt file expects the following environment variables
to be defined.

* BOOST_ROOT: Location of Boost (e.g. C:\\Boost)
* SDL2: Location of the SDL2 directory (e.g. C:\\SDL2)
* FREETYPE_DIR: Location of the Freetype library (e.g. C:\\Freetype)

In addition, if for some reason CMake cannot find the FBX SDK, you can set an
environment variable called FBX_ROOT to point to the root FBX folder.

## <a name="installing-osx-dependencies"></a>Installing OSX Dependencies

The best way to install the dependencies is via Homebrew. Go to http://brew.sh
and read the instructions on how to install Homebrew. Once installed, you can
type

	brew install cmake boost freetype libjpeg libpng sdl2

In a terminal to install all the dependencies except for the FBX SDK. The FBX
SDK needs to be installed manually (with an install wizard). As of this
writing, the most recent FBX SDK is version 2016.1. This can be downloaded
from

http://download.autodesk.com/us/fbx/20161/fbx20161_fbxsdk_clang_mac.pkg.tgz

You may need to edit some variables in the top-level CMakeLists.txt file
of the Gintonic project to get everything to work.

## <a name="installing-linux-dependencies"></a>Installing Linux Dependencies

In the top-level directory there's a file called `bootstrap-linux.sh`.
Run that file to install all dependencies.

# <a name="the-structure-of-the-engine"></a>The Structure of the Engine

My suggestion is to explore the code in the examples directory to get a feel
for how the various classes interact with eachother. Basically, there's a huge
singleton class in renderer.hpp that takes care of rendering. There are
vectors, matrices and quaternion classes in math.hpp.

Rendering geometry is done with the mesh class in mesh.hpp. However if you 
need simple geometric shapes you could look into basic_shapes.hpp. When 
rendering a mesh, you need to bind a material so that the correct shader gets
activated. This interplay is in the files materials.hpp and shaders.hpp. Every
class in shaders.hpp derives from the gintonic::opengl::shader class and adds
some extra methods that correspond to uniform variables in the corresponding
shader with the same name (as the class). Each material in materials.hpp
(again with the same naming scheme) then references its corresponding shader
class and sets up the uniform variables in the shader given its own data
members. Each new shader class needs to be known to the giant singleton class
in renderer.hpp (called gintonic::renderer). Currently, I just give it a new
static method that fetches the new shader class (and add a new static pointer
to the new class). This works for now, but could probably benefit from a more
generic approach.

I wrote the classes in math.hpp a very long time ago. I believe they could use
a rewrite using SSE intrinsics. The vector and matrix class being templated
is pointless.

The classes in lights.hpp represent a light in space. Currently, I have
implemented a directional light and a point light. Just as with materials,
each light has a corresponding shader class and a corresponding shader.

# <a name="the-math-in-gintonic"></a>The Math in Gintonic

We work over the field \f$\mathbb{R}\f$. The set \f$\mathbb{P}^n\f$ denotes \f$n\f$-dimensional projective space. The set \f$\mathbb{R}^n\f$ denotes the usual \f$n\f$-dimensional vector space. The set \f$\mathbb{A}^n\f$ denotes \f$n\f$-dimensional affine space. The vector dot product is denoted by \f$a \cdot b := \sum_{i=1}^n a_i b_i\f$ and in the three dimensional case the vector cross product is denoted by \f$a \times b\f$.

All types in `gintonic` live in the namespace `gintonic`. The code examples omit this namespace for brevity. Just assume we have

~~~~~~~~~~~~~~~{.cpp}
#include "gintonic.hpp"
using namespace gintonic;
~~~~~~~~~~~~~~~

at the top of every file.

# <a name="vector-spaces-and-projective-spaces"></a>Vector Spaces and Projective Spaces

**Definition**. For any two vectors \f$a,b \in \mathbb{R}^n\f$, the angle \f$\theta\f$ between \f$a\f$ and \f$b\f$ is defined by the equation \f$a \cdot b = |a| \cdot |b| \cdot \cos \theta\f$

Note in particular that if \f$a\f$ and \f$b\f$ are of unit length, then \f$a \cdot b = \cos \theta\f$. Thus to find the angle between arbitrary vectors in `gintonic` you can do

~~~~~~~~~~~~~~~{.cpp}
vec3f a = ...
vec3f b = ...
float angle = std::acos(dot(a,b) / (a.length() * b.length());
~~~~~~~~~~~~~~~

Taking the inverse cosine `std::acos` is quite slow (relatively speaking), so try to avoid it. In fact, usually you only really need the \f$\cos \theta\f$ term, and in the case of vectors of unit length that is then equal to `dot(a,b)`, which is really fast.

**Lemma**. Let \f$a,b \in \mathbb{R}^n\f$, with \f$b \neq 0\f$. The orthogonal projection of \f$a\f$ onto \f$b\f$ is given by \f$\frac{a\cdot b}{b \cdot b}b\f$.

*Proof*. If \f$a\f$ is orthogonally projected onto \f$b\f$, then that projected vector must be a multiple of \f$b\f$, i.e. it is of the form \f$\lambda b\f$ for some \f$\lambda \in \mathbb{R}\f$. Then the vector \f$a - \lambda b\f$ is orthogonal to \f$b\f$. That is, we have \f$(a - \lambda b) \cdot b = 0\f$. Solving for \f$\lambda\f$ gives the result. ∎

We denote the projection of \f$a\f$ onto \f$b\f$ by \f$a_{\|}\f$. It should be clear from the context onto which vector we project. We denote by \f$a_{\perp}\f$ the vector \f$a - a_{\|}\f$. We call \f$a_{\perp}\f$ the *rejection*. We say that we *reject* \f$a\f$ off \f$b\f$. From the above proof we see that \f$a_{\|} \cdot a_{\perp} = 0\f$ and \f$a = a_{\|} + a_{\perp}\f$. Note also in particular that if \f$a\f$ and \f$b\f$ are orthogonal, then \f$a_{\|} = 0\f$ and \f$a_{\perp} = a\f$.

`gintonic` has support for vector projections and rejections. The vector \f$b\f$, where \f$a\f$ will be projected on and rejected off, must be of unit length. The functions do not check if this is the case so you must do so yourself. Here's a code sample. Note that in the sample the vector \f$b\f$ is of unit length, so we do not have to normalize it.

~~~~~~~~~~~~~~~{.cpp}
vec3f a(1.0f, 2.0f, 3.0f);
vec3f b(0.0f, 1.0f, 0.0f); // The Y-axis.
vec3f c = project(a,b); // Will be {0,2,0}
vec3f d = reject(a,b); // Will be {1,0,3}
~~~~~~~~~~~~~~~

**Definition**. An element \f$P = (P_x : P_y : P_z : P_w) \in \mathbb{P}^3\f$ is called a *point* if \f$P_w \neq 0\f$. It is called a *direction* if \f$P_w = 0\f$.

This definition makes \f$\mathbb{P}^3\f$ into a disjoint union of its set of points and its set of directions.

**Definition**. Given two distinct points \f$P,Q \in \mathbb{P}^3\f$, the *direction from* \f$P\f$ *to* \f$Q\f$} is given by
\f[ \left( \frac{Q_x}{Q_w} - \frac{P_x}{P_w} :  \frac{Q_y}{Q_w} - \frac{P_y}{P_w} :  \frac{Q_z}{Q_w} - \frac{P_z}{P_w} : 0 \right). \f]
The notation is \f$Q-P\f$.

So this defines a map \f$\{P : P_w \neq 0\}^2 \to \{P : P_w = 0\}\f$ given by \f$(P,Q) \mapsto Q-P\f$. I do not know what properties this map has.

To be formal, in Gintonic we work in three-dimensional projective space \f$\mathbb{P}^3\f$. This is because OpenGL works in three-dimensional projective space. However, in practise points have their fourth coordinate set to \f$1\f$ and directions have their fourth coordinate set to \f$0\f$.

**Definition**. If \f$A\f$ is any invertible matrix, we call \f$A\f$ *orientation-preserving* if \f$\det A > 0\f$. We call \f$A\f$ *orientation-reversing* if \f$\det A < 0\f$.

We call the standard unit basis \f$ \{e_1, e_2, e_3\} \f$ of \f$\mathbb{R}^3\f$ right-handed. Any base change matrix with a negative determinant changes this right-handed orientation into a left-handed orientation.

# <a name="affine-transformations"></a>Affine Transformations

## <a name="affine-matrices">Affine Matrices

**Definition**. An *affine matrix* \f$A\f$ is a \f$4 \times 4\f$ matrix of the form
\f[ A = \begin{bmatrix}
	R_{11} & R_{12} & R_{13} & t_{x} \\
	R_{21} & R_{22} & R_{23} & t_{y} \\
	R_{31} & R_{32} & R_{33} & t_{z} \\
	0      &      0 &      0 & 1 \end{bmatrix} = \begin{bmatrix} R & t \\ 0 & 1 \end{bmatrix}, \f]
where \f$R\f$ is an invertible \f$3 \times 3\f$ matrix and \f$t \in \mathbb{R}^3\f$. We denote the set of affine matrices by \f$\AffMat_3\f$.

Note that \f$\AffMat_3 \subset \Mat_4\f$. There's a natural inclusion
\f[ \GL_3 \to \AffMat_3 , \qquad R \mapsto \begin{bmatrix} R & 0 \\ 0 & 1 \end{bmatrix}. \f]

**Lemma.** For a given affine matrix \f$A = \begin{bmatrix} R & t \\ 0 & 1 \end{bmatrix}\f$, the inverse \f$A^{-1}\f$ is given by
\f[ A^{-1} = \begin{bmatrix} R^{-1} & -R^{-1}t \\ 0 & 1 \end{bmatrix} \f]

*Proof.* Straightforward computation. ∎

**Lemma.** The product of two affine matrices is an affine matrix. The identity matrix is an affine matrix. Every affine matrix has an inverse. So \f$\AffMat_3\f$ is a non-abelian group under multiplication.

*Proof.* Straightforward computation. ∎

This also shows that
\f[ \GL_3 \subset \AffMat_3 \subset \GL_4 \subset \Mat_4 \f]
and note that all inclusions are strict. So affine matrices live between invertible \f$3 \times 3\f$ matrices and invertible \f$4 \times 4\f$ matrices. Affine matrices act on \f$\mathbb{P}^3\f$, as follows. For a given \f$P \in \mathbb{P}^3\f$ and \f$A \in \AffMat_3\f$ we simply have

\f[ \begin{align*}
A \cdot P &= \begin{bmatrix}
	R_{11} & R_{12} & R_{13} & t_{x} \\
	R_{21} & R_{22} & R_{23} & t_{y} \\
	R_{31} & R_{32} & R_{33} & t_{z} \\
	0      &      0 &      0 & 1 \end{bmatrix} \cdot \begin{bmatrix} P_x \\ P_y \\ P_z \\ P_w \end{bmatrix} \\
	&= \begin{bmatrix}
	R_{11} P_x + R_{12} P_y + R_{13} P_z + t_x P_w \\
	R_{21} P_x + R_{22} P_y + R_{23} P_z + t_x P_w \\
	R_{31} P_x + R_{32} P_y + R_{33} P_z + t_x P_w \\
	P_w
	\end{bmatrix} \in \mathbb{P}^3.
\end{align*} \f]

If \f$P\f$ is a point, we can set \f$P_w = 1\f$ and see that \f$A \cdot P\f$ first applies the matrix \f$R\f$ to \f$(P_x, P_y, P_z)\f$ and then adds a translation \f$t\f$ at the end. If \f$P\f$ is a direction, we see that the translation \f$t\f$ has no effect at all. Furthermore observe that points get mapped to points and directions get mapped to directions.

## <a name="scalings"></a>Scalings

We need to be able to move objects around in \f$\mathbb{P}^3\f$. Do to that, we can use affine matrices. There are three types of affine matrices prevalent in Gintonic. They are scale matrices, translation matrices and rotation matrices.

For a given vector \f$s = (s_x,s_y,s_z)\f$ with \f$s_x,s_y,s_z \neq 0\f$, an afine scaling matrix is a matrix of the form
\f[ \begin{bmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}. \f]
Points and directions simply get scaled by this matrix. There's not much else to say. `gintonic` has support for constructing an affine matrix with a given scaling. This is accomplished as follows.

	float sx = 1.0f;
	float sy = 2.0f;
	float sz = 0.5f;
	mat4f scalematrix(sx, sy, sz);

Note that you don't supply the scaling as a `vec3f`.

## <a name="translations"></a>Translations

For a given vector \f$t = (t_x, t_y, t_z)\f$, an affine translation matrix is a matrix of the form
\f[ \begin{bmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix}. \f]
For a point \f$P = (P_x : P_y : P_z : 1) \in \mathbb{P}^3\f$, we see that application of this matrix to \f$P\f$ gives \f$(P_x + t_x : P_y + t_y : P_z + t_z : 1)\f$. For a direction \f$D = (D_x : D_y : D_z : 0) \in \mathbb{P}^3\f$, we see that application of this matrix to \f$D\f$ gives us \f$D\f$ again.

Care must be taken when multiplying scaling and translation matrices. For example,
\f[ \begin{bmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix} = \begin{bmatrix} s_x & 0 & 0 & s_xt_x \\ 0 & s_y & 0 & s_yt_y \\ 0 & 0 & s_z & s_zt_z \\ 0 & 0 & 0 & 1 \end{bmatrix}, \f]
\f[ \begin{bmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} = \begin{bmatrix} s_x & 0 & 0 & t_x \\ 0 & s_y & 0 & t_y \\ 0 & 0 & s_z & t_z \\ 0 & 0 & 0 & 1 \end{bmatrix}. \f]
The general rule of thumb is to *first apply your scaling, then apply your translation}. Otherwise your intended translation gets scaled along with the scaling.

`gintonic` has support for constructing a matrix with a given translation. This is accomplished as follows.

~~~~~~~~~~~~~~~{.cpp}
vec3f translation(20.0f, 1000.0f, -10.0f);
mat4f transmatrix(translation);
~~~~~~~~~~~~~~~

Note that, in constrast with the construction of a scaling matrix, you need to supply the translation as a `vec3f`.

## <a name="rotations"></a>Rotations

We will now discuss rotation matrices. This is a big topic.

**Definition**. An orthogonal matrix is a matrix \f$Q\f$ such that \f$Q^\intercal Q = Q Q^\intercal = I\f$. The subgroup of all orthogonal matrices is denoted by \f$\OrthogonalGroup_n \subset \GL_n\f$.

Note that \f$Q^{-1} = Q^\intercal\f$.

**Definition**. A rotation matrix is an element of the kernel of the homomorphism \f$\det : \OrthogonalGroup_n \to \mathbb{R}^*\f$. The kernel is denoted by \f$\SO_n\f$.

Sometimes, matrices for which \f$R^\intercal = R^{-1}\f$ but such that \f$\det R = -1\f$ are called impure rotations. In our case, we only really care about the case \f$n=3\f$. Let us define three fundamental rotation matrices.

**Definition**. Let \f$\alpha \in \mathbb{R}\f$. The counterclockwise rotation of \f$\alpha\f$ radians around the \f$x\f$-axis is given by the matrix
\f[ R_x(\alpha) := \begin{bmatrix} 1 & 0 & 0 \\ 0 & \cos \alpha & -\sin \alpha \\ 0 & \sin \alpha & \cos \alpha \end{bmatrix}. \f]
The counterclockwise rotation of \f$\alpha\f$ radians around the \f$y\f$-axis is given by the matrix
\f[ R_y(\alpha) := \begin{bmatrix} \cos \alpha & 0 & -\sin \alpha \\ 0 & 1 & 0 \\ \sin \alpha & 0 & \cos \alpha \end{bmatrix}. \f]
The counterclockwise rotation of \f$\alpha\f$ radians around the \f$z\f$-axis is given by the matrix
\f[ R_y(\alpha) := \begin{bmatrix} \cos \alpha & -\sin \alpha & 0 \\ \sin \alpha & \cos \alpha & 0 \\ 0 & 0 & 1 \end{bmatrix}. \f]

**Lemma.** For any \f$\alpha \in \mathbb{R}\f$ we have \f$R_x(\alpha), R_y(\alpha), R_z(\alpha) \in \SO_3\f$.

*Proof.* Use that \f$\sin^2 \alpha + \cos^2 \alpha = 1\f$, and that \f$\cos(\alpha) = \cos(-\alpha)\f$, and \f$\sin(-\alpha) = -\sin(\alpha)\f$. ∎

Rotations in \f$3\f$-dimensional space have a peculiar property that is not seen in \f$2\f$ dimensions. This was first observed by Euler in 1776.

**Theorem A.** If \f$R \in \SO_3\f$, then \f$R\f$ has an eigenvalue of \f$1\f$.

Before we begin the proof, let us consider what this means and why this is special. If \f$R\f$ has an eigenvalue of \f$1\f$ then that means that there exists a non-zero vector \f$v\f$ such that \f$Rv = v\f$. So this vector is unaffected by the rotation \f$R\f$. Such a vector is what we call the *axis of rotation}. The theorem ensures us that every rotation has such an axis of rotation. In contrast, in \f$2\f$-dimensional space every rotation matrix is of the form
\f[ \begin{bmatrix} \cos \alpha & -\sin \alpha \\ \sin \alpha & \cos \alpha \end{bmatrix} \f]
and it is straightforward to see that the eigenvalues are \f$e^{\pm i\alpha} \in \mathbb{C}\f$ for \f$\alpha \in [0, 2 \pi)\f$, so not even real. We see that our standard rotations \f$R_x(\alpha), R_y(\alpha), R_z(\alpha)\f$ indeed have an eigenvalue of \f$1\f$.

*Proof of Theorem A.* The eigenvalues \f$\{\lambda\}\f$ of \f$R\f$ are defined by the equation \f$\det(R - \lambda I) = 0\f$. So it suffices to show that \f$\det(R-I) = 0\f$. Since \f$\det\f$ is a homomorphism we see that \f$\det(-R) = -\det(R)\f$ and \f$\det(R^{-1}) = 1/\det(R) = 1\f$. Furthermore recall that \f$\det(M^\intercal) = \det(M)\f$ for any matrix \f$M\f$. We find
\f[ \det(R-I) = \det\left((R-I)^\intercal\right) = \det \left(R^\intercal - I\right). \f]
Now we use that \f$R\f$ is a rotation, i.e. \f$R^\intercal = R^{-1}\f$. So,
\f[ \det \left(R^\intercal - I\right) = \det \left( R^{-1} - R^{-1}R \right) = \det \left( R^{-1} \right) \det \left(I-R\right) = -\det(R-I). \f]
Hence \f$\det(R-I) = -\det(R-I)\f$, so \f$\det(R-I) = 0\f$. ∎

## <a name="the-angle-axis-rotation"></a>The Axis-Angle Rotation

Now given a non-zero vector \f$u\f$, can we find a rotation matrix \f$R \in \SO_3\f$ that has this vector \f$u\f$ as the axis of rotation? If \f$u\f$ is not of unit length, then dividing \f$u\f$ by its length will not change the resulting rotation matrix \f$R\f$. So we assume that \f$u\f$ is of unit length. Choose an angle \f$\theta\f$ and suppose we want to rotate the vector \f$a\f$ around the axis \f$u\f$ counterclockwise by \f$\theta\f$ radians. Recall that we can project \f$a\f$ onto \f$u\f$ to get a vector \f$a_{\|}\f$. Furthermore we can reject \f$a\f$ off \f$u\f$ to get \f$a_{\perp}\f$. Hence \f$a = a_{\|} + a_{\perp}\f$ and \f$a_{\|} \cdot a_{\perp} = 0\f$. Let \f$w := u \times a\f$. Observe that
\f[ w = u \times a = u \times \left(a_{\|} + a_{\perp}\right) = u \times a_{\|} + u \times a_{\perp} = u \times a_{\perp}. \f]
The last equality is because \f$a_{\|}\f$ is a multiple of \f$u\f$. The key insight is that right now, \f$w\f$ can be seen as \f$a_{\perp}\f$ rotated around \f$u\f$ by \f$90\f$ degrees. In the plane defined by \f$u\f$ as normal vector (which contains \f$a_{\perp}\f$), we can rotate \f$a_{\perp}\f$ by \f$\theta\f$ radians by the equation \f$a_{\perp} \cos \theta + w \sin \theta\f$. Denote by \f$b\f$ the rotated vector \f$a\f$ around the axis \f$u\f$ by \f$\theta\f$ radians. Project and reject \f$b\f$ onto \f$u\f$, i.e. let \f$b = b_{\|} + b_{\perp}\f$. Observe that \f$b_{\perp} = a_{\perp} \cos \theta + w \sin \theta\f$. Since \f$b_{\|}\f$ is a multiple of \f$u\f$, it is unaffected by the rotation. Hence \f$b_{\|} = a_{\|}\f$. Hence we find
\f[ b = a_{\|} + a_{\perp} \cos \theta + (u \times a_{\perp}) \sin \theta. \f]
Substitution of the formulas for \f$a_{\|}\f$ and \f$a_{\perp}\f$ gives
\f[ b = a \cos \theta + (u \times a) \sin \theta + u \cdot (u \cdot a) \cdot (1- \cos \theta). \f]
If \f$u = (u_1,u_2,u_3)\f$, Define a matrix \f$U\f$ by
\f[ U := \begin{bmatrix} 0 & -u_3 & u_2 \\ u_3 & 0 & -u_1 \\ -u_2 & u_1 & 0 \end{bmatrix}. \f]
One can show that \f$Uv = u \times v\f$ for any \f$v \in \mathbb{R}^3\f$. That is, \f$U\f$ is the matrix representation of the linear map \f$u \times \cdot : \mathbb{R}^3 \to \mathbb{R}^3\f$. The last equation for \f$b\f$ then becomes
\f[ \begin{align*} 
b &= a \cos \theta + Ua \sin \theta + u \cdot (u \cdot a) \cdot (1- \cos \theta) \\
&= a - a + a\cos \theta + Ua \sin \theta + u \cdot (u \cdot a) \cdot (1- \cos \theta) \\
&= a - a(1 - \cos \theta) + Ua \sin \theta + u \cdot (u \cdot a) \cdot (1- \cos \theta) \\
&= a  + Ua \sin \theta +\left(u(u \cdot a) - a\right) (1- \cos \theta).
\end{align*} \f]
Recall the vector triple product:
\f[ x \times (y \times z) = y(x \cdot z) - z(x \cdot y), \qquad x,y,z \in \mathbb{R}^3. \f]
Setting \f$x=y=u\f$ and \f$z=a\f$, we get \f$u \times (u \times a) = u(u \cdot a) - a(u \cdot u)\f$. Since \f$u\f$ is of unit length, \f$u \cdot u = 1\f$. Hence \f$u \times (u \times a) = u(u\cdot a) - a\f$, or in other words \f$U^2 a = u(u\cdot a) - a\f$. Substitution into the last equation gives us
\f[ b = a + Ua \sin \theta + U^2 a (1- \cos \theta). \f]
Hence,
\f[ b = R \cdot a, \qquad R = I + U \sin \theta + U^2 (1-\cos \theta). \f]
It can be shown that \f$R\f$ is indeed a rotation matrix, i.e. that \f$R^\intercal = R^{-1}\f$ and \f$\det R = 1\f$.

The classes `gintonic`:mat3f` and `gintonic`:mat4f` have constructors that take a unit axis and an angle, and construct exactly the above matrix from this information. The supplied axis is not checked for unit length, you must do so yourself. Here's an example on how to initialize a matrix with a given axis and angle.

~~~~~~~~~~~~~~~{.cpp}
// Define the rotation axis.
// Don't forget to make it unit length!
vec3f axis = vec3f(1.0f, 2.0f, 3.0f).normalize();

// Define the rotation angle.
float angle = 3.1415f;

mat3f rot3(axis, angle);

mat4f rot4(axis, angle);
~~~~~~~~~~~~~~~

## <a name="the-look-at-rotation"></a>The ``Look At'' Rotation

In OpenGL by default, the \f$x\f$-axis is to the right, the \f$y\f$-axis goes up, and the \f$z\f$-axis comes towards you. That means that by default, the viewer looks down the negative \f$z\f$-axis. By convention, we say that \f$[0,0,-1]\f$ is the default *forward direction}.

It is extremely benificial to have another sort of rotation constructor for matrices. We answer the following question: Given two points \f$e,s \in \mathbb{R}^3\f$ can we construct an affine matrix \f$A\f$ that is centered at \f$e\f$ and looks at \f$s\f$? The letter \f$e\f$ is for *eye* and the letter \f$s\f$ is for *subject*. By default the eye looks down the negative \f$z\f$-axis. The direction from \f$e\f$ to \f$s\f$ is given by \f$s-e\f$. Let \f$f := (s-e) / \|s-e\|\f$. Then \f$f\f$ is a unit length direction vector pointing from \f$e\f$ to \f$s\f$. We want a rotation matrix that rotates the default forward vector \f$[0,0,-1]\f$ to the vector \f$f\f$.

Now we run into a problem, since there is more than one rotation matrix that does the job. That is, once we have oriented the vector \f$[0,0,-1]\f$ to point to \f$f\f$, there's ambiguity on what is the ``up'' direction. For this reason we also require an up direction. Let us denote this vector by \f$u\f$. Usually, \f$u\f$ is taken to be \f$[0,1,0]\f$. We will assume that \f$u\f$ is of unit length. Then a vector orthogonal to both \f$f\f$ and \f$u\f$ is given by \f$r := f \times u\f$. Let \f$u' := r \times f\f$. Then the matrix
\f[ R = [r , u' , -f] \f]
is an orthogonal matrix, since \f$r\f$, \f$u'\f$ and \f$f\f$ are orthogonal to each other. If we apply the default forward vector \f$[0,0,-1]\f$ to \f$R\f$, we get \f$R \cdot [0,0,-1]^\intercal = f\f$, as desired. The default right vector \f$[1,0,0]\f$ gets mapped to \f$R \cdot [1,0,0]^\intercal = r\f$ and the default up vector \f$[0,1,0]\f$ gets mapped to \f$R \cdot [0,1,0]^\intercal = u'\f$.

*Remark.* The matrix \f$R\f$ is an impure rotation (\f$\det R = -1\f$). I do not know why we cannot use a pure rotation. It is probably because OpenGL chose \f$[0,0,-1]\f$ as the default forward vector instead of \f$[0,0,1]\f$.

The matrix \f$R\f$ transforms the standard basis \f$\{e_1, e_2, -e_3\}\f$ to the basis \f$\{r,u',f\}\f$. We want to go the other way: We need to orient our objects so that OpenGL can munch the vertices in its standard unit cube defined by \f$\{e_1, e_2, -e_3\}\f$. That matrix is simply given by \f$R^{-1}\f$. Since \f$R\f$ is orthogonal we have \f$R^{-1} = R^\intercal\f$.

Now let
\f[ A := \begin{bmatrix} R & -e \\ 0 & 1 \end{bmatrix} \in \AffMat_3. \f]
This matrix changes the basis \f$\{e_1, e_2, -e_3\}\f$ into the basis \f$\{r,u',f\}\f$ and adds a translation of \f$-e\f$. That is, if we are positioned at \f$e\f$ then we will be positioned at \f$0\f$ after applying this matrix (which is what we need for OpenGL). As before, we need the inverse of this affine matrix so that we transform the basis \f$\{r,u',f\}\f$ into the basis \f$\{e_1,e_2,-e_3\}\f$. So our final matrix is
\f[ \label{eq:lookatmatrix} A^{-1} = \begin{bmatrix} R^\intercal & -R^\intercal e \\ 0 & 1 \end{bmatrix} 
= \begin{bmatrix}
r_x & r_y & r_z & - r \cdot e \\
u'_x & u'_y & u'_z & - u' \cdot e \\
-f_x & -f_y & -f_z & f \cdot e \\
0 & 0 & 0 & 1
\end{bmatrix}. \f]
`gintonic` supplies a `mat4f` constructor that takes an eye position, a subject position and and up direction. It will construct exactly the matrix above. Here's a code sample.

~~~~~~~~~~~~~~~{.cpp}
vec3f eye_location(10.0f, 20.0f, -40.0f);
vec3f subject_location(4.0f, 3.0f, -5.0f);
vec3f up_dir(0.0f, 1.0f, 0.0f); // almost always the Y-axis.
mat4f lookat(eye_location, subject_location, up_dir);
~~~~~~~~~~~~~~~

There are some problems with this approach. Consider when \f$u\f$ is very close to \f$f\f$. For example, the user could supply the up direction as \f$[0,1,0]\f$, the eye location \f$e\f$ could be at \f$[0,0,0]\f$ and the object that we want to look at is located at \f$s = [0,100,0]\f$. Then \f$f = (s-e) / \|s-e\| = [0,1,0] = u\f$. Then \f$r = f \times u = u \times u = 0\f$. Then the algorithm continues with \f$u' = r \times f = 0 \times f = 0\f$. Thus the matrix will become
\f[ \begin{bmatrix}
0 & 0 & 0 & 0 \\
0 & 0 & 0 & 0 \\
0 & -1& 0 & e_y \\
0 & 0 & 0 & 1
\end{bmatrix} \not\in \AffMat_3. \f]
Therefore some care must be taken at singular points (i.e. when \f$\pm f = u\f$). The solution that solves this problem is to use quaternions.

## <a name="quaternions"></a>Quaternions

**Definition**. The set \f$\mathbb{H} := \{w + xi + yj + zk : w,x,y,z \in \mathbb{R}\}\f$ together with the algebraic relations \f$i^2 = j^2 = k^2 = ijk = -1\f$ is called the set of quaternions.

It can be shown that \f$ij = k\f$, \f$jk = i\f$, \f$ki = j\f$, \f$ji = -k\f$, \f$kj = -i\f$ and \f$ik = -j\f$. It can also be shown that \f$\mathbb{H}\f$ is a division ring. The multiplication of two quaternions is rather verbose, so we won't spell it out here. `gintonic` has support for quaternions:

~~~~~~~~~~~~~~~{.cpp}
quatf q(1.0f, 0.0f, 1.0f, 0.0f); // q = 1 + j
quatf r(1.0f, 1.0f, 0.0f, 1.0f); // r = 1 + i + k
quatf s = q * r;
~~~~~~~~~~~~~~~

**Definition**. If \f$q = w+xi+yj+zk \in \mathbb{H}\f$, then the *conjugate* of \f$q\f$ is defined as \f$\overline{q} := w - xi -yj - zk\f$.


**Definition**. If \f$q = w+xi+yj+zk \in \mathbb{H}\f$, we define the *norm* of \f$q\f$ as \f$|q| := \sqrt{q \cdot \overline{q}}\f$.

It can be shown that \f$|\cdot|\f$ is indeed a norm.

**Definition**. We define the set of unit quaternions as \f$\mathbb{V} := \{q \in \mathbb{H} : |q| = 1\}\f$.


**Lemma.** If \f$q \in \mathbb{V}\f$, then \f$q^{-1} = \overline{q}\f$.

*Proof.* Write \f$q = w + xi + yj + zk\f$. Then we have \f$\overline{q} \cdot q = q \cdot \overline{q} = w^2 + x^2 + y^2 + z^2 = |q|^2 = 1\f$. ∎

So what's the deal with these quaternions? Let us make them act on vectors. First, let us say that a quaternion which has a \f$w\f$-component of \f$0\f$ is called a *pure quaternion*. Denote this set by \f$\mathbb{H}_P\f$ (so, \f$\mathbb{H}_P = \mathbb{H} \setminus \mathbb{R} \cup \{0\}\f$). This set carries a structure of a vector space, but multiplication can fall out of \f$\mathbb{H}_P\f$, since \f$i^2 = -1 \not\in \mathbb{H}_P\f$ for instance. Define a map
\f[ Q: \mathbb{R}^3 \to \mathbb{H}_P, \qquad \begin{bmatrix} x \\ y \\ z \end{bmatrix} \mapsto xi + yj + zk. \f]
This map is a vector space isomorphism with inverse given by \f$xi +yj + zk \mapsto [x,y,z]^\intercal\f$. Once could call it the "quaternification" of a vector, or the "vectorization" of a pure quaternion for the inverse.

**Theorem.** Let \f$q \in \mathbb{H}\f$ and let \f$v \in \mathbb{H}_P\f$. Then \f$q \cdot v \cdot q^{-1} \in \mathbb{H}_P\f$.

*Proof.* First, just as with complex numbers note that \f$q^{-1} = \overline{q} / |q|^2\f$. Multiplication by real numbers is closed under \f$\mathbb{H}_P\f$, so we may assume without loss of generality that \f$q \in \mathbb{V}\f$, and hence \f$q^{-1} = \overline{q}\f$. Writing out the multiplication is left as an exercise :-). ∎

The theorem together with our vector space isomorphism shows that we have an action
\f[\phi : \mathbb{V} \times \mathbb{R}^3 \to \mathbb{R}^3, \qquad (q,v) \mapsto Q^{-1} \left( q \cdot Q(v) \cdot \overline{q} \right). \f]
In fact, quaternions ``are'' rotations.

**Theorem.** For any \f$q \in \mathbb{V}\f$, we have \f$\phi(q, \cdot) \in \SO_3\f$.

*Proof.* Let \f$q \in \mathbb{V}\f$ and write \f$R := \phi(q, \cdot)\f$. First we show that \f$R\f$ is actually a linear map. Let \f$a,b \in \mathbb{R}^3\f$, \f$\lambda \in \mathbb{R}\f$. We have
\f[ R(a+b) = \phi(q,a+b) = Q^{-1} \left( q \cdot Q(a+b) \cdot \overline{q} \right) \f] \f[= Q^{-1}\left( q Q(a) \overline{q} \right) + Q^{-1} \left( q Q(b) \overline{q} \right) = Ra + Rb. \f]
Similarly, \f$R(\lambda a) = \lambda Ra\f$. Consider the standard basis \f$\{e_1,e_2,e_3\} \subset \mathbb{R}^3\f$. Write \f$q = w+xi+yj+zk\f$. We have
\f[ \begin{align*}
Re_1 &= Q^{-1} \left( q \cdot i \cdot \overline{q} \right) \\
&= Q^{-1} \left( \left(w+xi+yj+zk\right) \cdot i \cdot \left( w -xi-yj -zk \right) \right) \\
&= Q^{-1} \left( \left(w+xi+yj+zk\right) \cdot \left(x + wi +zj -yk\right) \right) \\
&= Q^{-1} \left(0 + i\left(w^2+x^2-y^2-z^2\right) + 2j\left(xy + zw\right) +2k\left(xz -yw\right)\right) \\
&= \begin{bmatrix} w^2+x^2-y^2-z^2 \\ 2xy +2zw \\ 2zx - 2yw \end{bmatrix} = \begin{bmatrix} |q|^2 -2y^2-2z^2 \\ 2xy +2zw \\ 2zx - 2yw \end{bmatrix} = \begin{bmatrix} 1-2y^2-2z^2 \\ 2xy +2zw \\ 2zx - 2yw \end{bmatrix}
\end{align*} \f]
In a similar manner, we can compute \f$R e_2\f$ and \f$R e_3\f$ too. Thus, the matrix represenation of \f$R\f$ with respect to the standard basis is given by
\f[ R = \begin{bmatrix}
1 - 2y^2 -2z^2 & 2xy - 2zw & 2xz + 2yw \\
2xy + 2zw & 1 - 2x^2 -2z^2 & 2yz -2xw \\
2xz -2yw & 2yz + 2xw & 1-2x^2 -2y^2
\end{bmatrix}. \f]
One can then readily verify that \f$RR^\intercal = R^\intercal R = I\f$, and that \f$\det R = 1\f$. ∎

`gintonic` can convert a quaternion into an affine matrix, because `mat4f` has a constructor that takes a `quatf` that builds exactly the matrix constructed in the above theorem. The quaternion is expected to be of unit length. This is not checked by the matrix constructor, so you have to do so yourself. Here's a code example.

~~~~~~~~~~~~~~~{.cpp}
quatf q(1.0f, 2.0f, 3.0f, 4.0f);
q.normalize(); // Make q of unit length.
mat4f rot(q); // Creates a rotation matrix.
~~~~~~~~~~~~~~~

Just as with regular rotation matrices, we will answer the question: given a unit axis \f$u\f$ and a rotation angle \f$\theta\f$, what is the corresponding quaternion? It turns out that the remarkably elegant

\f[ q := \cos\left( \frac{\theta}{2} \right) + u\sin \left( \frac{\theta}{2} \right) \f]
does the job, where \f$u\f$ is now viewed as \f$u = u_xi + u_yj + u_zk\f$. One can verify that the matrix represenation corresponding to \f$q\f$ is given by
\f[ I + U\sin\theta + U^2 (1-\cos \theta),\f]
the same result we found when we searched for a matrix that rotates by \f$\theta\f$ radians around a unit vector \f$u\f$.

`gintonic` can construct a `quatf` given a unit axis and angle of rotation. Here's a code example.

~~~~~~~~~~~~~~~{.cpp}
vec3f axis(1.0f, 2.0f, 3.0f);
axis.normalize();
float angle = deg2rad(40.0f);
quatf q = quatf::axis_angle(axis, angle);
~~~~~~~~~~~~~~~

*Remark.* There's not yet a ``look at'' constructor for `quatf`. I have a tremendous amount of trouble doing this, and I'm not sure why. Singularities galore.

## <a name="putting-it-all-together"></a>Putting It All Together

So, we have scaling, translation and rotation matrices. Let \f$S\f$ be a scaling matrix, let \f$T\f$ be a translation matrix and let \f$R\f$ be a rotation matrix (all \f$4 \times 4\f$ matrices). The way you want to apply your matrices is then \f$T \cdot R \cdot S\f$. In other words, for a point \f$P \in \mathbb{P}^3\f$ you first scale it, then you rotate it, and then you translate it. Again, for directions \f$D \in \mathbb{P}^3\f$, the translation matrix \f$T\f$ has no effect.

Projections
===========

So far we described \f$\AffMat_3\f$. These transformations change points and directions. OpenGL does not care about these transformations. The way OpenGL renders is it takes all the supplied triangles located inside the unit cube \f$[-1,1]^3 \subseteq \mathbb{R}^3\f$ and renders these triangles. When you apply the usual affine scaling, rotation and translation matrices \f$T \cdot R \cdot S\f$, you will experience a very distorted image. There's a fourth final matrix \f$P\f$ required to get the desired ``natural'' looking effect. This is a big topic too but not very interesting per se (or maybe I just can't be bothered), so just know that
\f[ P := \begin{bmatrix} 
\frac{w/h}{\tan(\theta / 2)} & 0 & 0 & 0 \\
0 & w/h & 0 & 0 \\
0 & 0 & \frac{n+f}{n-f} & \frac{2nf}{n-f} \\
0 & 0 & -1 & 0  
 \end{bmatrix} \f]
does the job, where \f$w\f$ is the width of the viewport, \f$h\f$ is the height of the viewport, \f$\theta\f$ is the field of view (FOV), \f$n\f$ is the nearplane and \f$f\f$ is the farplane. Note that \f$P \not\in \AffMat_3\f$, but \f$P \in \GL_4\f$. This particualr matrix is called a *perspective} projection, since it makes objects that are far away smaller, and objects that are closer bigger. In other words, it transforms a frustum (a pyramid with the top chopped off) into a cube. The name ``projection'' matrix is kind of a misnomer, since \f$P^2 \neq P\f$, i.e. \f$P\f$ is not idempotent. In fact \f$P\f$ is invertible, since one can also transform a cube into a frustum.

TODO: Add stuff about perspective projections and orthographic projections.

Coordinate Systems
==================

In \f$\mathbb{R}^n\f$, we have the notion of a basis. In \f$\mathbb{P}^n\f$, we can define a coordinate system.
**Definition**. Choose a point \f$O = (O_1,\ldots,O_n) \in \mathbb{R}^n\f$ and choose a basis \f$\{b_1,\ldots,b_n\}\f$ of \f$\mathbb{R}^n\f$. Then \f$(O : 1) \in \mathbb{P}^n\f$ is a point and each \f$(b_i : 0) \in \mathbb{P}^n\f$ is a direction. The tuple \f$((O : 1), (b_1 : 0), \ldots, (b_n : 0))\f$ is called a *coordinate system}.

So a coordinate system is nothing more than a basis of \f$\mathbb{R}^n\f$ together with a selected origin \f$O\f$. By abuse of notation we switch between the vector space representation and the projective space representation when it suits us. If \f$\left(O, \{b_1,b_2,b_3\}\right)\f$ and \f$\left(O', \{b'_1, b'_2, b'_3\} \right)\f$ are two coordinate systems, then there exists an affine matrix \f$A \in \AffMat_3\f$ that changes the former coordinate system into the latter. Indeed, if \f$M\f$ is the \f$3 \times 3\f$ matrix that transforms the vector space basis \f$\{b_1,b_2,b_3\}\f$ into \f$\{b_1',b_2',b_3'\}\f$, then \f$A\f$ is given by
\f[ A = \begin{bmatrix} M & O'-M\cdot O \\ 0 & 1 \end{bmatrix}. \f]

In `gintonic` (and most other engines), we have three basic coordinate systems that we keep track of.

* `MODEL` space, also called `LOCAL` space. This is the coordinate system of a mesh where the origin is at \f$0\f$ and we use the standard OpenGL axes \f$\{e_1,e_2,e_3\} \subset \mathbb{R}^3\f$. The vertices of every mesh are given in this coordinate space.
* `WORLD` space. This is the coordinate system where all logical calculations take place. The coordinate axes are \f$\{e_1,e_2,e_3\}\f$.
* `VIEW` space, also called `CAMERA} space, also called `EYE} space. This is the coordinate system where the ``eye'', or ``you'', or ``the camera'', is located at \f$0\f$. The coordinate axes depend on the orientation of the ``eye'' or ``camera''. This is the coordinate system that you use in shaders.
* `CLIP` space, also called `HOMOGENEOUS` space. This is a space contained in a unit cube \f$[-1,1]^3\f$. OpenGL only understands this coordinate system, all the other coordinate systems are our own invention.
* `SCREEN` space. This is not really a coordinate system; and we don't really interact with it. It's the final rectangle that OpenGL renders to so that you can view stuff on your screen.
* `NORMAL` space. More on this later. Normal space only acts on directions, so we don't need an origin.
* `TANGENT` space. More on this later. Tangent space only acts on directions, so we don't need an origin.

When rendering a mesh (together with a ``material'' on it), we need to transform the mesh from `MODEL` space all the way to `CLIP` space so that OpenGL takes care of the rest. To spell it out, we need two affine matrices \f$M,V \in \AffMat_3\f$ and a projection matrix \f$P \in \GL_4\f$ such that for every vertex position \f$x\f$ of the mesh, the transformed vertex position \f$P\cdot V \cdot M \cdot x\f$ goes like
\f[ `MODEL` \xrightarrow{M} `WORLD` \xrightarrow{V} `VIEW` \xrightarrow{P} `CLIP` \f]
and thus ends up in clip space. Note that \f$Mx\f$ gives the world space coordinates and \f$VMx\f$ gives the eye space coordinates.
In `gintonic`, the singleton class `renderer} manages the current \f$M\f$ matrix, the current \f$V\f$ matrix and the current \f$P\f$ matrix. The \f$V\f$ matrix and the \f$P\f$ matrix only change once per frame. The \f$M\f$ matrix can change many times per frame, depending on how many meshes are rendered. The matrices can be accessed as follows.

~~~~~~~~~~~~~~~{.cpp}
// Get a constant reference to the current model->world matrix.
const auto& M = renderer::matrix_M();

// Get a constant reference to the current world->view matrix.
const auto& V = renderer::matrix_V();

// Get a constant reference to the current view->clip matirx.
const auto& P = renderer::matrix_P();

// Get a constant reference to the current model->view matrix.
const auto& VM = renderer::matrix_VM();

// Get a constant reference to the current model->clip matrix.
const auto& PVM = renderer::matrix_PVM();
~~~~~~~~~~~~~~~

You cannot set the \f$V\f$, \f$P\f$, \f$VM\f$ and \f$PVM\f$ matrices directly.
The \f$V\f$ matrix is updated automatically at every frame in the `renderer::update} static method.
You can, however, set the \f$M\f$ matrix with the static method `renderer::set\_model\_matrix}. When you set the \f$M\f$ matrix, the \f$VM\f$ and \f$PVM\f$ matrices are updated automatically, so you don't have to worry about those. For instance,

~~~~~~~~~~~~~~~{.cpp}
// Start with the identity matrix as the model matrix.
renderer::set_model_matrix(1.0f);
// Get the PVM matrix.
mat4f pvm1 = renderer::matrix_PVM();
vec3f axis(0.0f, 1.0f, 0.0f);
float angle = M_PI / 5.0f;
mat4f rotmat(axis, angle);
mat4f transmat(vec3f(4.0f, 0.0f, 5.0f));
mat4f modelmat = transmat * rotmat;
// Update the model matrix with something else.
renderer::set_model_matrix(modelmat);
// Get the PVM matrix.
mat4f pvm2 = renderer::matrix_PVM();
// pvm1 and pvm2 are two different matrices!
assert(pvm1 != pvm2);
~~~~~~~~~~~~~~~

SQT Transforms
==============

To capture the three different transformation matrices (scalings, translations and rotations), we pack the relevant information in a struct. In `gintonic` we call this struct an `SQT}. This struct carries a `vec3f` for the scale, a `vec3f` for the translation and a `quatf` for the rotation. The size of an `SQT} is \f$12\f$ bytes. Why do we not just use a `mat4f`? After all a `mat4f` is the natural mathematical object for a transformation. Well, it is hard to change a rotation or translation once encoded in a `mat4f`. If we know that \f$M = T \cdot R \cdot S\f$ is a given affine matrix, where \f$T\f$ is a translation, \f$R\f$ a rotation and \f$S\f$ a scaling, and we want to modify translation, then we would first need to multiply by \f$T^{-1}\f$ from the left and then multiply by a new translation \f$T'\f$ by the left to alter the translation. It gets even more cumbersome when we want to change the rotation. If you keep the relevant information separate, it is easy to change those values. Meanwhile, you can always convert to a `mat4f` when needed. In the code sample below we manage an `SQT} transform, update the translation and then easily obtain a modified `mat4f`.

~~~~~~~~~~~~~~~{.cpp}
SQT sqt;
sqt.scale = {1.0f, 1.0f, 1.0f};
sqt.translation = {20, -30, 40};
sqt.rotation = quatf::axis_angle(vec3f(0.0f, 1.0f, 0.0), M_PI);

// Create an affine matrix from an SQT
mat4f mat1(sqt);

// Easily alter the translation.
sqt.translation = {-30, 10, 50};

// Easily update the affine matrix.
mat1 = mat4f(sqt);
~~~~~~~~~~~~~~~

Everything Is An Entity
=======================

We need a class that represents an ``object'' in the world, also called an ``entity'' or ``actor''. We could attack this problem with inheritance and OOP, starting out with an entity that does almost nothing. It turns out that this creates problems. Here is an excerpt of the inheritance diagram of the Unreal Engine (taken from *Game Engine Architecture*, Jason Gregory 2009).

~~~~~~~~~~~~~~~{.cpp}
class Actor;
class Brush : public Actor;
// more brushes ...
class Light : public Actor;
// more lights ...
class Controller : public Actor;
class AIContoller : public Controller;
class PlayerController : public Controller;
// more controllers ...
class Inventory : public Actor;
class Ammunition : public Inventory;
class Powerups : public Inventory;
class Weapon : public Inventory;
// more inventory ...
class Pawn : public Actor;
class Vehicle : public Pawn;
class UnrealPawn : public Pawn;
class RedeemerWarhead : public Pawn;
class Scout : public Pawn;
// more pawns ...
~~~~~~~~~~~~~~~

Suppose that we extend this inheritance tree with the following classes.

~~~~~~~~~~~~~~~{.cpp}
class LandVehicle : public Vehicle;
class Car : public LandVehicle;
class Truck : public LandVehicle;
class Motorcycle : public LandVehicle;
class WaterVehicle : public Vehicle;
class Yacht : public WaterVehicle;
class Hovercraft : public WaterVehicle;
class SpeedBoat : public WaterVehicle;
~~~~~~~~~~~~~~~

At some point later in time, someone tells us that we need amphibious vehicles. Such a vehicle does not fit into the existing taxonomic system. This may cause the programmers to panic or, more likely, to ``hack'' their class hierarchy in various ugly and error-prone ways.

One solution is to utilize C++'s multiple inheritance feature. However multiple inheritance in C++ poses a number of practical problems.
Suppose we do

~~~~~~~~~~~~~~~{.cpp}
class AmphibiousVehicle 
: public LandVehicle
, public WaterVehicle;
~~~~~~~~~~~~~~~

Then `AmphibiousVehicle` contains *two copies* of `Vehicle`. This can be solved by virtual inheritance:

~~~~~~~~~~~~~~~{.cpp}
class LandVehicle : public virtual Vehicle;
class WaterVehicle : public virtual Vehicle;
~~~~~~~~~~~~~~~

Now `AmphibiousVehicle` contains only one copy of `Vehicle`, the compiler takes care of that. However, since the inheritance is virtual, the constructor of `AmphibiousVehicle` must explicitly state which constructor of `Vehicle` must be invoked as well as which constructor of `LandVehicle` and which of `WaterVehicle`. So we get a giant mess of constructors everywhere. The design becomes error-prone. This could be remedied by giving `Vehicle` a default constructor with no arguments; then the compiler will just choose that default constructor when we specify no base constructor in the constructor of `AmphibiousVehicle`. However if `Pawn` has no default constructor then it's probably impossible to give `Vehicle` a default constructor.

The way to solve all this is to use no inheritance at all and use a ``component''-based design. We start out with a `entity` class that has a few *optional* pointers to component classes. In `gintonic`, an excerpt of the `entity` class is as follows.

~~~~~~~~~~~~~~~{.cpp}
class entity
{
public:
	// ...

	mesh*       mesh_component       = nullptr;
	material*   material_component   = nullptr;
	light*      light_component      = nullptr;
	rigid_body* rigid_body_component = nullptr;
	logic*      logic_component      = nullptr;
	AI*         AI_component         = nullptr;
	camera*     camera_component     = nullptr;
	proj_info*  proj_info_component  = nullptr;

	const SQT& local_transform() const noexcept;
	const SQT& global_transform() const noexcept;

	void add_child(entity&);
	void remove_child(entity&);
	void set_parent(entity&);
	void unset_parent();

	void set_scale(const vec3f&) noexcept;
	void multiply_scale(const vec3f&) noexcept;
	void set_translation(const vec3f&) noexcept;
	void add_translation(const vec3f&) noexcept;
	void set_rotation(const quatf&) noexcept;
	void post_multiply_rotation(const quatf&) noexcept;
	void pre_multiply_rotation(const quatf&) noexcept;
	void set_local_transform(const SQT&) noexcept;
	void post_add_local_transform(const SQT&) noexcept;
	void pre_add_local_transform(const SQT&) noexcept;
}
~~~~~~~~~~~~~~~

**TO BE CONTINUED.** -->
