#ifndef ZIP_HPP
#define ZIP_HPP

#include "iterator_range.hpp"
#include "iterbase.hpp"

#include <tuple>
#include <iterator>
#include <type_traits>

namespace iter {
    template <typename ... Rest>
    class zip_iter;

    template <typename ... Containers>
    auto zip(Containers && ... containers) ->
	  iterator_range<zip_iter<typename 
	  iter::weakest_iterator<decltype(std::begin(containers))...>::value,
	  decltype(std::begin(containers))...>>
    {
        auto begin =
		  zip_iter<typename 
				   iter::weakest_iterator<decltype(std::begin(containers))...>::value,
				   decltype(containers.begin())...>(std::begin(containers)...);

        auto end =
		  zip_iter<typename 
				   iter::weakest_iterator<decltype(std::end(containers))...>::value,
				   decltype(containers.end())...>(std::end(containers)...);
		
        return iterator_range<decltype(begin)>(begin,end);
    }

  template<typename ... Types>
  //std::swap won't work with the temporary tuple r-values returned by operator *()
  //so, we need this class to write a specialized swap that will be found using ADL
  struct tupleWrapper{
	std::tuple<Types...> data;
	tupleWrapper(std::tuple<Types...> _data) : data{_data}
	{}
	operator std::tuple<Types...> () {return data;}
	std::tuple<Types...>& asTuple() {return data;}
  };

  //accept by value if necessary
  template<typename ... Types>
  void swap(tupleWrapper<Types ...> t1, tupleWrapper<Types ...> t2){
	std::swap(t1.data, t2.data);
  }
  //accept by reference if possible
  template<typename ... Types>
  void swap(tupleWrapper<Types ...>& t1, tupleWrapper<Types ...>& t2){
	std::swap(t1.data, t2.data);
  }
  
  //concatenate wrapped tuple lists
  template<typename T1, typename ... T2s>
  tupleWrapper<T1, T2s...> wrapperCat(tupleWrapper<T1> t1, tupleWrapper<T2s...> t2){
	return tupleWrapper<T1, T2s...>{std::tuple_cat(t1.asTuple(), t2.asTuple())};
  };


  //By value is probably OK, since these are just a few pointers
  template<int N, typename ...Ts>
  auto get(tupleWrapper<Ts...> tw)->decltype(std::get<N>(tw.data)){
	  return std::get<N>(tw.data);
  }

  template <typename WeakestTag, typename Iterator>
  class zip_iter<WeakestTag, Iterator> {
        private:
            Iterator iter;

        public:
	//always return a tuple so that imap works properly
	//users should always expect a tuple as value type from zip anyway
	//and its a tuple OF REFERENCES
	using value_type = tupleWrapper<decltype(*iter)>;//
	//should give a correctly const qualified lvalue reference
	//		  typename 
	//				 std::add_lvalue_reference<typename 
	//										   std::iterator_traits<Iterator>::value_type>::type>;//decltype(*iter);
	using iterator_category = WeakestTag;
	using difference_type = ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;

            zip_iter(const Iterator & i) :
                iter(i){ }

	value_type operator*() {
	  return value_type(*iter);//std::forward_as_tuple(*iter);
            }
	const value_type operator*() const {
	  return value_type(*iter);
	}

            zip_iter & operator++() {
                ++iter;
                return *this;
            }
	  //to be STL compatible we're supposed to provide postincrement too
	  	  zip_iter operator++(int) {
		zip_iter tmp(*this);
		operator++();
		return tmp;
	  }

	  
	  //implement decrement for bidirectional iterators
	template <typename U = WeakestTag>
	  typename std::enable_if<
		std::is_base_of<std::bidirectional_iterator_tag,
						U>::value,
		zip_iter&>::type
	  operator --(){
	  static_assert(std::is_same<U, WeakestTag>::value,
					"don't provide template params for operator --");
		--iter;
		return *this;
	  }
	template<typename U = WeakestTag>
	  typename std::enable_if<
		std::is_base_of<std::bidirectional_iterator_tag,
						U>::value,
		zip_iter> operator --(int) {
		static_assert(std::is_same<U, WeakestTag>::value,
					  "don't provide template params for operator --");

		zip_iter tmp(*this);
		operator--();
		return tmp;
	  }
	  
	//random access operators
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							zip_iter&>::type
	operator +=(N n){
	  iter += n;
	  return *this;
	}

	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							zip_iter&>::type
	operator -=(N n){
	  iter -= n;
	  return *this;
	}
	template <typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							difference_type>::type
	operator -(const zip_iter& other){
	  static_assert(std::is_same<U, WeakestTag>::value, 
					"don't provide template parametesr to operator -");
	  //since iters are in lockstep, only look at the first
	  return iter - other.iter; 
	}

	
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::Value,
							value_type&>::type
	operator[](N n){
	  return *(iter + n);
	}
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							const value_type&>::type
	operator[](N n) const {
	  return *(iter + n);
	}

            bool operator!=(const zip_iter & rhs) const {
                return (this->iter != rhs.iter);
            }
	//stl compatible iterators should implement == as well
	bool operator ==(const zip_iter& rhs) const {
	  return this->iter == rhs.iter;
	}

	//comparison ops for random access iterators
	template <typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
								   U>::value,
				   bool>::type
	operator <(const zip_iter& other) const {
	  static_assert(std::is_same<U, WeakestTag>::value,
					"don't supply template params to operator <");
	  return this->iter < other.iter;
	}
  };

#if 0
    template <typename First, typename Second>
        struct zip_iter<First,Second> {

        private:
            First iter1;
            Second iter2;

        public:
            using Elem1_t = decltype(*iter1);
            using Elem2_t = decltype(*iter2);
            zip_iter(const First & f, const Second & s) :
                iter1(f),iter2(s) { }

            auto operator*() -> decltype(std::forward_as_tuple(*iter1,*iter2))
            {
                return std::forward_as_tuple(*iter1,*iter2);
            }
            zip_iter & operator++() {
                ++iter1;
                ++iter2;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter1 != rhs.iter1) && (this->iter2 != rhs.iter2);
            }
        };
#endif
    //this specialization commented out 
  template <typename WeakestTag, typename First, typename ... Rest>
  class zip_iter<WeakestTag, First,Rest...> {
        private:
            First iter;
	zip_iter<WeakestTag, Rest...> inner_iter;
            
        public:
	using elem_type = tupleWrapper<decltype(*iter)>;

	using value_type = 
	  decltype(wrapperCat(elem_type(*iter), *inner_iter));


	using iterator_category = WeakestTag; 
	using difference_type = ptrdiff_t;
	using pointer = value_type*;
	using reference = value_type&;

            zip_iter(const First & f, const Rest & ... rest) :
                iter(f),
                inner_iter(rest...) {}

	//most iterators operator* return a reference.  Since our tuples are ephemeral
	//we have to return a tuple of references by value
	//std::swap does NOT work with temporary tuples (even those containing references)
	//hence the extra wrapperCat type, whose swap can be specialized by ADL
	value_type operator*()
            {
			  return wrapperCat(elem_type(*iter), *inner_iter);
            }

	const value_type operator*() const {
	  return wrapperCat(elem_type(*iter), *inner_iter);
	}

            zip_iter & operator++() {
                ++iter;
                ++inner_iter;
                return *this;
            }
	  zip_iter  operator++(int) {
		zip_iter tmp(*this);
		operator++();
		return tmp;
	  }
	  
	//decrement for bidirectional iterators
	template<typename U = WeakestTag>
	typename std::enable_if<
	  std::is_base_of<std::bidirectional_iterator_tag,
					  U>::value,
	  zip_iter&>::type
	operator --() {
	  static_assert(std::is_same<U, WeakestTag>::value, 
					"don't specity template params for operator --");
	  --iter;
	  --inner_iter;
	  return *this;
	}

	
	template <typename U = WeakestTag>
	typename std::enable_if<
	  std::is_base_of<std::bidirectional_iterator_tag,
					  U>::value,
	  zip_iter>::type
	operator --(int) {
	  static_assert(std::is_same<U, WeakestTag>::value, 
					"don't specity template params for operator --");
	  
	  zip_iter tmp(*this);
	  operator--();
	  return tmp;
	  
	}

	//random access operators
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							zip_iter&>::type
	operator +=(N n){
	  iter += n;
	  inner_iter += n;
	  return *this;
	}

	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							zip_iter&>::type
	operator -=(N n){
	  iter -= n;
	  inner_iter -= n;
	  return *this;
	}
	template <typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							difference_type>::type
	operator -(const zip_iter& other){
	  //since iters are in lockstep, only look at the first
	  static_assert(std::is_same<U, WeakestTag>::value, 
					"don't provide template parametesr to operator -");
	  return iter - other.iter; 
	}
	
	
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::Value,
							value_type&>::type
	operator[](N n){
	  return std::tuple_cat(std::forward_as_tuple(*(iter + n)), *(inner_iter + n));
	}
	template <typename N, typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
											U>::value,
							const value_type&>::type
	operator[](N n) const {
	  return std::tuple_cat(std::forward_as_tuple(*(iter + n)), *(inner_iter + n));
	}
					  

	//I don't understand why the condition here is and, not or
            bool operator!=(const zip_iter & rhs) const {
			  return (this->iter != rhs.iter) && 
                    (this->inner_iter != rhs.inner_iter);
            }
	//stl compatible iterators should implement == as well
	bool operator ==(const zip_iter& rhs) const {
	  return this->iter == rhs.iter &&
		this->inner_iter == rhs.inner_iter;
	}

	//comparison ops for random access iterators
	template <typename U = WeakestTag>
	typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
								   U>::value,
				   bool>::type
	operator <(const zip_iter& other) const {
	  static_assert(std::is_same<U, WeakestTag>::value,
					"don't supply template params to operator <");
	  //I'm going to ignore the inner_iters, since hopefully everything is in lockstep
	  return this->iter < other.iter;
	}
	
    };

  //addition operations for random access iterators
  template <typename N, typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  zip_iter<WeakestTag, Rest...> >::type
  operator +(zip_iter<WeakestTag, Rest...> z, N n){
	zip_iter<WeakestTag, Rest...> ret(z);
	ret += n;
	return ret;
  }
  template <typename N, typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  zip_iter<WeakestTag, Rest...> >::type
  operator +(N n, zip_iter<WeakestTag, Rest...> z){
	return z + n;
  }

  //subtraction operations for random access iterators
  //
  template <typename N, typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  zip_iter<WeakestTag, Rest...> >::type
  operator -(zip_iter<WeakestTag, Rest...> z, N n){
	zip_iter<WeakestTag, Rest...> ret(z);
	ret -= n;
	return ret;
  }
  
  //other comparison operators for random access iterators
  template<typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  bool>::type
  operator >(const zip_iter<WeakestTag, Rest...>& lhs,
			  const zip_iter<WeakestTag, Rest...>& rhs){
	return rhs < lhs;
  }
  template<typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  bool>::type
  operator <=(const zip_iter<WeakestTag, Rest...>& lhs,
			  const zip_iter<WeakestTag, Rest...>& rhs){
	return ! operator>(lhs, rhs);
  }
  template<typename WeakestTag, typename ... Rest>
  typename std::enable_if<std::is_base_of<std::random_access_iterator_tag,
										  WeakestTag>::value,
						  bool>::type
  operator >=(const zip_iter<WeakestTag, Rest...>& lhs,
			  const zip_iter<WeakestTag, Rest...>& rhs){
	return !(lhs < rhs);
  }
	


}

//these default to iterator members, use those instead
/*namespace std {
    template <typename ... Containers>
        struct iterator_traits<iter::zip_iter<Containers...>> {
            using difference_type = ptrdiff_t;
            using iterator_category = 
			  typename iter::zip_iter<Containers...>::tag_type;
	};
	}*/
#endif //ZIP_HPP
