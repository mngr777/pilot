Server runs simple physical simulation using Open Dynamics Engine.
Objects are created by running Scheme scripts from data/worlds
(currently hardcoded to run data/worlds/test.scm).

Client renders objects using OpenSceneGraph library.

Client/server communication is implemented using Boost.Asio library.
GNU Guile is used for running Scheme code and interpreting network messages.
