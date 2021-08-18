/* CG Mini Project - Travelling Salesman Problem 3D Visualization
   Yash Aryan Chopra
   1BG18CS128
*/
#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

//point data structure
typedef struct {
    int x, y, z;
}point;

point* arrayNodes = NULL;	    //to store the Nodes positions
int nNodes;					    //number of Nodes
int* position = NULL;			//order to visit the Nodes
double** distances;				//a pre-computed array of distances between Nodes (to speedup)
double initialDistance;			//a value to print at the end of the running

 
class RenderState {
public:
    float mouseX, mouseY, cameraX, cameraY;
    bool mouseLeftDown, mouseRightDown;
    RenderState() {
        this->mouseX = 0;
        this->mouseY = 0;
        this->mouseLeftDown = false;
        this->mouseRightDown = false;
        this->cameraX = 0.0f;
        this->cameraY = 0.0f;
    }

};

RenderState rs;


void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    RenderState* rsp = new RenderState();
    rs = *rsp;
}


void exit() {
    delete& rs;
}


void drawCoordinates() {

    // draw the axis lines
    glLineWidth(1);
    glColor4f(1.0, 0.0, 0.0, 0.5); // red x
    glBegin(GL_LINES);

    // x axis
    glVertex3f(-4.0, 0.0f, 0.0f);
    glVertex3f(4.0, 0.0f, 0.0f);

    glVertex3f(4.0, 0.0f, 0.0f);
    glVertex3f(3.0, 1.0f, 0.0f);

    glVertex3f(4.0, 0.0f, 0.0f);
    glVertex3f(3.0, -1.0f, 0.0f);
    glEnd();

    // y axis
    glColor4f(0.0, 1.0, 0.0, 0.5); // green y
    glBegin(GL_LINES);
    glVertex3f(0.0, -4.0f, 0.0f);
    glVertex3f(0.0, 4.0f, 0.0f);

    glVertex3f(0.0, 4.0f, 0.0f);
    glVertex3f(1.0, 3.0f, 0.0f);

    glVertex3f(0.0, 4.0f, 0.0f);
    glVertex3f(-1.0, 3.0f, 0.0f);
    glEnd();

    // z axis
    glColor4f(0.0, 0.0, 1.0, 0.5); // blue z
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0f, -4.0f);
    glVertex3f(0.0, 0.0f, 4.0f);


    glVertex3f(0.0, 0.0f, 4.0f);
    glVertex3f(0.0, 1.0f, 3.0f);

    glVertex3f(0.0, 0.0f, 4.0f);
    glVertex3f(0.0, -1.0f, 3.0f);
    glEnd();

    //draw path
    glColor3ub(128, 0, 128); //purple path
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    for (int k = 0; k < nNodes; k++) {
        glVertex3i(arrayNodes[position[k]].x, arrayNodes[position[k]].y, arrayNodes[position[k]].z);
        //printf("%d, %d, %d\n", arrayNodes[position[k]].x, arrayNodes[position[k]].y, arrayNodes[position[k]].z);
    }
    glEnd();

    //draw points/nodes
    glPointSize(8);
    glColor3ub(238, 130, 238); //violet nodes
    for (int k = 0; k < nNodes; k++)
    {
        glBegin(GL_POINTS);
        glVertex3i(arrayNodes[k].x, arrayNodes[k].y, arrayNodes[k].z);
       // printf("%d, %d %d\n", arrayNodes[k].x, arrayNodes[k].y, arrayNodes[k].z);
        glEnd();
    }

    glLineWidth(1);
    glFlush();

}


void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    glLoadIdentity();
    glTranslatef(0, 0, -10.0f);
    glRotatef(rs.cameraX, 1, 0, 0);
    glRotatef(rs.cameraY, 0, 1, 0);

    glScalef(1.0, 2.0, 1.0);
    glutWireCube(1.0);
    drawCoordinates();

    glFlush();

}


void mouseCallback(int button, int state, int x, int y)
{
    rs.mouseX = x;
    rs.mouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            rs.mouseLeftDown = true;
        }
        else if (state == GLUT_UP)
            rs.mouseLeftDown = false;
    }
}


void mouseMotionCallback(int x, int y)
{
    if (rs.mouseLeftDown)
    {
        rs.cameraY += (x - rs.mouseX);
        rs.cameraX += (y - rs.mouseY);
        rs.mouseX = x;
        rs.mouseY = y;
    }
}


void idleCallback()
{
    glutPostRedisplay();
}


double distanceFinder(point a, point b)
{
    return sqrt(pow(static_cast<__int64>(a.x) - static_cast<__int64>(b.x), 2.0) + pow(static_cast<__int64>(a.y) - static_cast<__int64>(b.y), 2.0) + pow(static_cast<__int64>(a.z) - static_cast<__int64>(b.z), 2.0));
}


double computeDistance()
{
    double distancePath = 0.0;
    for (int k = 0; k < nNodes - 1; k++)
        distancePath += distances[position[k]][position[k + 1]];
    distancePath += distances[position[nNodes - 1]][position[0]];
    return distancePath;
}


void Swap2Elements(int* e1, int* e2)
{
    do
    {
        *e1 = rand() % nNodes;
        *e2 = rand() % nNodes;
    } while (*e1 == *e2);
    int temp = position[*e1];
    position[*e1] = position[*e2];
    position[*e2] = temp;
}


void getShortestPath()
{
    int k = 0;
    int limit = 10000;
    while (k < 25000 && limit > 0)
    {
        int e1, e2;
        double d1 = computeDistance();
        Swap2Elements(&e1, &e2);
        double d2 = computeDistance();
        if (d2 > d1)
        {
            //revert the swaps
            int temp = position[e1];
            position[e1] = position[e2];
            position[e2] = temp;
            limit--;
        }
        else
            printf("Distance iteration-%d : %lf\n", k, computeDistance());
        k++;
    }
}


void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    else if (key == 'a')
    {
        getShortestPath();
        printf("Initial distance: %lf; Shortest path: %lf\n", initialDistance, computeDistance());
    }
    else if (key == 's')
    {
        int e1, e2;
        double d1 = computeDistance();
        Swap2Elements(&e1, &e2);
        double d2 = computeDistance();
        if (d2 > d1)
        {
            //revert the swaps
            int temp = position[e1];
            position[e1] = position[e2];
            position[e2] = temp;
        }
        printf("Distance: %lf \n", computeDistance());

    }

    glutPostRedisplay();
}


void Init(int size) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_POINT_SMOOTH);

    nNodes = size;		 // set the number of Nodes given by the user
    srand(time(NULL));	//to start each execution as different

    //dynamically create the arrays
    //arrayNodes = new point[size];
    arrayNodes = (point*)malloc(size * sizeof(point));
    position = (int*)malloc(size * sizeof(int));
    distances = (double**)malloc(size * sizeof(double*));

    // determine the Nodes position on the screen
    for (int k = 0; k < size; k++)
    {
        distances[k] = (double*)malloc(size * sizeof(double));
        int ri = rand() % (8);
        ri -= 8 / 2;
        arrayNodes[k].x = ri;
        ri = rand() % (8);
        ri -= 8 / 2;
        arrayNodes[k].y = ri;
        ri = rand() % (8);
        ri -= 8 / 2;
        arrayNodes[k].z = ri;
        position[k] = k;
    }

    //computes the distance from all Nodes to all Nodes
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
            distances[x][y] = distanceFinder(arrayNodes[x], arrayNodes[y]);	

    initialDistance = computeDistance();
    printf("Initial distance: %lf\n", initialDistance);
}


int main(int argc, char** argv)
{
    int n;
    printf("How many nodes do you need: ");
    scanf_s("%d", &n);
    printf("\n");
    printf("Press 's' to swap two random nodes if there is shorter path\n");
    printf("Press 'a' to find optimal path through all nodes\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idleCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutKeyboardFunc(onKeyDown);

    glViewport(0, 0, (GLsizei)500, (GLsizei)500);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    glMatrixMode(GL_MODELVIEW);

    Init(n);
    glutMainLoop();
    exit();
    return 0;
}
