/*

MIT License

Copyright (c) 2020, R. Gregor Weiß, Benjamin Ries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#ifndef CNN_ARGUMENT_H
#define CNN_ARGUMENT_H

#include <string>
#include <vector>

#include <sstream>
#include <cassert>
#include <ostream>

template <typename TP> class Argument {

protected:
        int pos_;
        int nargs_;

        std::vector<std::string>* id_;

        std::string descr_;

        TP* data_;

        bool parsed;

public:
        Argument();
        Argument(const std::string& , TP const& , int const& nargs = 0);
        Argument(const std::string& , const std::string& , TP const& , int const& nargs = 0 );
        /*
         * for positional arguments
         */
        Argument(int const& , const std::string& , TP const& );
        /*
         * copy constructor
         */
        Argument(Argument<TP> const& );
        ~Argument();

        Argument<TP>& operator=(Argument<TP> const& cp);

        int& size();
        int const& size() const;

        std::vector<std::string>& id();
        std::vector<std::string>& id() const;

        std::string& id(unsigned const& );
        std::string& id(unsigned const& ) const;

        int add_id(const std::string& );

        std::string& description();
        std::string& description() const;

        int& pos();
        int const& pos() const;

        int parse(int argc, char* argv[]);

        bool is(const std::string& );
        bool is_parsed();

        TP& val(int const& i = 0);
        TP const& val(int const& i = 0) const;

        friend std::ostream& operator<<(std::ostream& out, Argument<TP>& v) {
                out << "# " << v.id()[0] << " =";
                for (int i = 0; i < v.nargs_; i ++) {
                        out << " " << v.val(i);
                } /* i */
                out << std::endl;
                return out;
        }

private:

        void count_nargs_(int const& i, int argc, char* argv[]);
        int get_nargs_();
        void treat_default_sval();
};


template <typename TP> Argument<TP>::Argument() :
    pos_(0),
    nargs_(0),
    id_(new std::vector<std::string>() ),
    data_(new TP[std::max<int>(this->nargs_, 1)]),
    parsed(false) {

}

template <typename TP> Argument<TP>::Argument(Argument<TP> const& cp) :
    pos_(cp.pos_),
    nargs_(cp.nargs_),
    id_(new std::vector<std::string>(*cp.id_) ),
    data_(new TP[std::max<int>(this->nargs_, 1)]),
    parsed( cp.parsed ) {

    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = cp.data_[i];
    }
}

template <typename TP> Argument<TP>::Argument(const std::string& sid, TP const& sval, int const& nargs) :
    pos_(0),
    nargs_(nargs),
    id_(new std::vector<std::string>(1, sid) ),
    data_(new TP[std::max<int>(this->nargs_, 1)]),
    parsed(false) {

    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = sval;
    } /* i */
}

template <typename TP> Argument<TP>::Argument(const std::string& sid, const std::string& sid2, TP const& sval, int const& nargs) :
    pos_(0),
    nargs_(nargs),
    id_(new std::vector<std::string>(1, sid) ),
    data_(new TP[std::max<int>(this->nargs_, 1)]),
    parsed(false) {

    this->id_->push_back(sid2);

    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = sval;
    } /* i */
}


template <typename TP> Argument<TP>::Argument(int const& pos, const std::string& sid, TP const& sval) :
    pos_(pos),
    nargs_(0),
    id_(new std::vector<std::string>(1, sid) ),
    data_(new TP[std::max<int>(this->nargs_, 1)]),
    parsed(false) {

    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = sval;
    }
}

template <typename TP> Argument<TP>::~Argument() {
    delete this->id_;
    delete[] this->data_;
}

template <typename TP> Argument<TP>& Argument<TP>::operator=(Argument<TP> const& cp) {
    if (this == &cp) {
        return *this;
    }

    delete this->id_;
    delete[] this->data_;

    this->pos_ = cp.pos_;
    this->nargs_ = cp.nargs_;
    this->id_ = new std::vector<std::string>(*cp.id_);
    this->data_ = new TP[std::max<int>(cp.nargs_, 1)];

    for (int i = 0; i < std::max<int>(this->nargs_, 1); i++) {
        this->data_[i] = cp.data_[i];
    } /* i */

    return *this;
}


template <typename TP> std::vector<std::string>& Argument<TP>::id() {
    return *this->id_;
}

template <typename TP> std::vector<std::string>& Argument<TP>::id() const {
    return *this->id_;
}

template <typename TP> std::string& Argument<TP>::id(unsigned const& i) {
    return (*this->id_)[i];
}

template <typename TP> std::string& Argument<TP>::id(unsigned const& i) const {
    return (*this->id_)[i];
}

template <typename TP> int Argument<TP>::add_id(const std::string& add) {
    this->id_->push_back(add);
    return 0;
}

template <typename TP> std::string& Argument<TP>::description() {
    return this->descr_;
}

template <typename TP> std::string& Argument<TP>::description() const {
    return this->descr_;
}

template <typename TP> int& Argument<TP>::size() {
    return this->nargs_;
}

template <typename TP> int const& Argument<TP>::size() const {
    return this->nargs_;
}

template <typename TP> int& Argument<TP>::pos() {
    return this->pos_;
}

template <typename TP> int const& Argument<TP>::pos() const {
    return this->pos_;
}

template <typename TP> bool Argument<TP>::is(const std::string& str) {
    bool result = 0;
    for (std::vector<std::string>::iterator it = this->id_->begin(); it != this->id_->end(); it++) {
        if (*it == str)
            result = 1;
    }
    return result;
}

template <typename TP> bool Argument<TP>::is_parsed() {
    return parsed;
}

template <typename TP> void Argument<TP>::count_nargs_(int const& i, int argc, char* argv[]) {

    std::stringstream inss(std::stringstream::in | std::stringstream::out);

    int k = i;
    while (k + 1 < argc) {
        std::string arg;
        inss.clear();
        inss << argv[k + 1];
        inss >> arg;
        if (arg.find("-") != std::string::npos) { break; }
        else if (arg.find("eof") != std::string::npos) { break; }
        else if (arg.find(">") != std::string::npos) { break; }

        this->nargs_++;
        k++;
    }
    this->nargs_++;
    delete[] this->data_;
    this->data_ = new std::string[this->nargs_];
}




template <typename TP> int Argument<TP>::get_nargs_() {
    return this->nargs_;
}

/*
 * \brief parse argument array
 *
 */
template <typename TP> int Argument<TP>::parse(int argc, char* argv[]) {
    if (this->pos_ == 0) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);

        for (int i = 0; i < argc; i++) {
            std::string arg;
            inss.clear();
            inss << argv[i];
            inss >> arg;
            if (this->is(arg)) {
                /* assert that option argument exists */
                assert(i + this->nargs_ < argc);
                for (int j = 0; j < this->nargs_; j++) {
                    inss.clear();
                    inss << argv[i + j + 1];
                    inss >> this->data_[j];
                } /* j */
            }
        } /* i */
    } else if (argc > this->pos_) {
        std::stringstream inss(std::stringstream::in | std::stringstream::out);
        inss << argv[this->pos_];
        inss >> this->data_[0];
    }

    parsed = true;
    return 0;
}

template <typename TP> TP& Argument<TP>::val(int const& i) {
        assert(i >= 0 && i < std::max<int>(this->nargs_, 1));
        return this->data_[i];
}

template <typename TP> TP const& Argument<TP>::val(int const& i) const {
        assert(i >= 0 && i < std::max<int>(this->nargs_, 1));
        return this->data_[i];
}

#endif //CNN_ARGUMENT_H
