/*
 * WrapperOf.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/11/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef WRAPPEROF_HPP_
#define WRAPPEROF_HPP_

namespace tscbpt {

/**
 * Class employed to identify the Decorator or Wrapper classes employed
 * to add functionality to their base classes.
 * This class may be employed to promote static types or templates to
 * its base class (the wrapped type), to avoid redefinition of classes
 * (e.g. Traits) for each wrapper or decorator class.
 */
template<typename T>
struct WrapperOf {
	typedef T		wrapped_type;
};

} /* namespace tscbpt */
#endif /* WRAPPEROF_HPP_ */
