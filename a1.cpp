void initScene(int argc, char *argv[])
{
    // load the image from a jpeg disk file to main memory
    heightmapImage = new ImageIO();
    if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
    {
        cout << "Error reading image " << argv[1] << "." << endl;
        exit(EXIT_FAILURE);
    }
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // do additional initialization here...
    pipelineProgram = new BasicPipelineProgram();
    pipelineProgram->Init(shaderBasePath);
    pipelineProgram->Bind();

    GLuint program = pipelineProgram->GetProgramHandle();

    
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), NULL, GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);
    
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    const void * offset = (const void*)0;
    GLsizei stride = 0;
    GLboolean normalized = GL_FALSE;
    
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, stride, offset);
    
    loc = glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(loc);
    offset = (const void*) sizeof(positions);
    glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
    glBindVertexArray(0);
    
    
}

void displayFunc()
{
    //render some stuff...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pipelineProgram->Bind();
    glBindVertexArray(vao);
    
    GLint first = 0;
    GLsizei count = 3;
    glDrawArrays(GL_TRIANGLES, first, count);
    
    glBindVertexArray(0);
    glutSwapBuffers();
    
    
}