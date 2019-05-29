%module libcontactangle

%include <opencv.i>
%cv_instantiate_all_defaults

%{
	#include "contactangle.hpp"
%}

%include "contactangle.hpp"