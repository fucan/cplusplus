/* c language */
typedef struct point3d
{
    float x;
    float y;
    folat z;
}Point3d;

#define Point3d_print( pd ) \
    printf("(%g, %g, %g )",pd->x,pd->y,pd->z );

void Point3d_print(const Point3d *pd)
{
    printf("(%g, %g, %g )",pd->x,pd->y,pd->z);
}

/*c++ definition */
class Point3d
{
    public:
        Point3d( float x=0.0,float y=0.0 ,float z= 0.0 )
            :_x(x),_y(y),_z(z)
        {

        }
        float x() {return _x;}
        float y() {return _y;}
        float z() {return _z;}

        void x(float xval) {_x=xval;}

        //... etc ...
    private:
        float _x;
        float _y;
        float _z;
};

inline ostream&
operator<<( ostream &os,const Point3d &pt )
{
    os<< "(" <<pt.x() <<", "
        << pt.y() <<", " << pt.z() << " )";
};

/*or by Point Point2d Point3d*/

class Point {
    public:
        Point(float x=0.0):_x(x) {}

        float x() {return _x;}

        void x(float xval){_x=xval;}
        //...
    protected:
        float _x;
};

class Point2d :public Point {
    public:
        Point2d(float x=0.0,float y=0.0)
            :Point(x),_y(y) {}

        float y(){return _y;}
        void y(float yval) {_y=yval;}
    protected:
        float _y;
};

class Point3d:public Point2d {
    public:
        Point3d( float x=0.0,float y=0.0,float z=0.0)
            :Point2d(x,y),_z(z) { }
        float z() {return _z;}
        void z(float zval) {_z=zval;}
        //...
    protected:
        float _z;
};

/*c++ template */

template<class type>
class Point3d {
    public:
        Point3d(type x=0.0,type y=0.0,type z=0.0)
            :_x(x),_y(y),_z(z) {}

        type x() {return _x;}
        void x(type xval) {_x=xval;}

        //... etc ...
    private:
        type _x;
        type _y;
        type _z;
};
/*also can set the point num*/
template <class type,int dim>
class Point {
    public:
        Point();
        Point( type coords[dim] ) {
            for( int index =0; index < dim; index++ ) {
                _coords[index] =coords[ index ];
            }
        }

        type& operator[] ( int index ) {
            assert( index < dim && index >= 0 );
            return _coords[ index ];
        }

        type operator [] ( inde index ) const {
            /*same as non-const instance*/
            assert ( index < dim && index >= 0);
            return _coords[ index ];
        }
        //... etc ...

    private:
        type _coords[ dim ];
};

inline
template <class type,int dim >
ostream&
operator<<( ostream &os,const Point<type,dim> &pt ) {
    os << "( ";
    for ( int ix =0;ix <dim-1; ix++ ) {
        os<<pt[ ix ] << ", ";
    }
    os << pt[ dim-1 ];
    os << " )";
}

int main()
{
    //Point3d point;
}
