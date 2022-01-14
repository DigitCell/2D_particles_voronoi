#include "solver.hpp"

Solver::Solver()
{
   // function_image.texture_create_empty();
   // function_image.height=settings.resolution;
   // function_image.width=settings.resolution;

    worldSize=glm::vec2(worldX, worldY);
    radius=particleRadius_init;

    worldElementsAmount=static_cast<int>(worldSize.x*worldSize.y);


    if(mapDivider>particleRadius_init)
    {
        mapCellSize=worldSize/static_cast<float>(mapDivider);
        mapSize.x=static_cast<int>(worldSize.x/mapCellSize.x);
        mapSize.y=static_cast<int>(worldSize.y/mapCellSize.y);
    }

    mapElementsAmount=mapSize.x*mapSize.y;

    // Define number of levels for the colormap quantization. This value should be in [1, 255]; otherwise, it will be cropped to 1 or 255.
    const unsigned int num_levels = 10;
    colorStateMap.clear();
    // Get the mapped color. Here, Parula is specified as the colormap.
    for(int i=0; i<9;i++)
    {
        const tinycolormap::Color color = tinycolormap::GetQuantizedColor(i/10.f, num_levels, tinycolormap::ColormapType::Parula);
        colorStateMap.push_back(glm::vec4(color.r(),color.g(),color.b(),1.0f));
    }

    setup();
}

void Solver::createProgram()
{
    dataProcessor.shaders.pointsPhysics= ComputeProgram("PointPhysics.glsl", calculateOptimalDisptachSpace(pointsNumber, 1, 1), false);
    CHECKGLERROR();
    dataProcessor.shaders.mapUpdate= ComputeProgram("MapUpdate.glsl", calculateOptimalDisptachSpace(pointsNumber, 1, 1), false);
    CHECKGLERROR();
    dataProcessor.shaders.mapClear= ComputeProgram("MapClear.glsl", calculateOptimalDisptachSpace(mapSize.x*mapSize.y, 1, 1), false);
    CHECKGLERROR();
    dataProcessor.shaders.neightbUpdate= ComputeProgram("NeightUpdate.glsl", calculateOptimalDisptachSpace(pointsNumber, 1, 1), false);
    CHECKGLERROR();
    dataProcessor.shaders.neightbSort= ComputeProgram("NeightSort.glsl", calculateOptimalDisptachSpace(pointsNumber, 1, 1), false);
    CHECKGLERROR();
    dataProcessor.shaders.writeToTexture= ComputeProgram("WriteToTexture.glsl", calculateOptimalDisptachSpace(pointsNumber, 1, 1), false);
    CHECKGLERROR();
    DispatchParams densityDispatch = calculateOptimalDisptachSpace((int)(worldSize.x),(int)(worldSize.y), 1);
    dataProcessor.shaders.distMap= ComputeProgram("distMapToTexture.glsl",  densityDispatch,  false);
    CHECKGLERROR();
   // DispatchParams densityDispatch2 = calculateOptimalDisptachSpace((int)(worldSize.x),(int)(worldSize.y), 1);
    DispatchParams densityDispatch2 = calculateOptimalDisptachSpace(worldElementsAmount, 1, 1);
    dataProcessor.shaders.distMapClear= ComputeProgram("distMapToTextureClear.glsl",  densityDispatch2,  false);
    CHECKGLERROR();
    //distMapToTextureDraw
    DispatchParams densityDispatchDraw1D = calculateOptimalDisptachSpace(worldElementsAmount, 1, 1);
    dataProcessor.shaders.distMapDraw= ComputeProgram("distMapToTextureDrawExp3.glsl",  densityDispatchDraw1D,  false);
    //  dataProcessor.shaders.distMapClear= ComputeProgram("distMapToTextureClear.glsl",  calculateOptimalDisptachSpace(worldElementsAmount, 1, 1), false);
    CHECKGLERROR();

    hasProgram = true;
}

void Solver::createBuffers()
{
    dataProcessor.buffers.pointData =    Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, pointsNumber*sizeof(PointData));
    CHECKGLERROR();
    dataProcessor.buffers.mapData =      Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, mapSize.x*mapSize.y*(cellMaxParticles+2)*sizeof(int));
    CHECKGLERROR();
    dataProcessor.buffers.mapSize =      Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, mapSize.x*mapSize.y*sizeof(int));
    CHECKGLERROR();
    dataProcessor.buffers.mapSizeFloat = Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, mapSize.x*mapSize.y*sizeof(float));
    CHECKGLERROR();
    dataProcessor.buffers.pointPos =     Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, pointsNumber*4*sizeof(float));
    CHECKGLERROR();
    dataProcessor.buffers.neightb =      Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, pointsNumber*neightbMaxParticles*sizeof(int));
    CHECKGLERROR();
    dataProcessor.buffers.neightbSort =  Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, pointsNumber*neightbMaxParticles*sizeof(int));
    CHECKGLERROR();
    dataProcessor.buffers.neightb_size = Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, pointsNumber*sizeof(int));
    CHECKGLERROR();
    dataProcessor.buffers.neightbInfo =  Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, pointsNumber*neightbMaxParticles*sizeof(NeightbData));
    CHECKGLERROR();

    dataProcessor.buffers.distMapData= Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_COPY, (int)(1.25*worldElementsAmount)*sizeof(VoronoiData));
    CHECKGLERROR();

    dataProcessor.buffers.ColorMapData = Buffer(GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW, colorStateMap.size()*4*sizeof(float));
    CHECKGLERROR();

    //boids_state = Buffer(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, sizeof(int));
   // CHECKGLERROR();

    hasBuffers = true;
}


void Solver::updateDispatchParams()
{
    dataProcessor.shaders.pointsPhysics.dispatchParams = calculateOptimalDisptachSpace(pointsNumber, 1, 1);

}

void Solver::resizeBoidBuffer()
{
    dataProcessor.buffers.pointData.resize(pointsNumber*sizeof(PointData));
    dataProcessor.buffers.mapData.resize(mapSize.x*mapSize.y*cellMaxParticles*sizeof(int));
    dataProcessor.buffers.mapSize.resize(mapSize.x*mapSize.y*sizeof(int));
    dataProcessor.buffers.pointPos.resize(pointsNumber*3*sizeof (float));

}

void Solver::deleteProgram()
{
    glDeleteProgram(dataProcessor.shaders.pointsPhysics.id);
    glDeleteProgram(dataProcessor.shaders.mapClear.id);
    glDeleteProgram(dataProcessor.shaders.mapUpdate.id);
    glDeleteProgram(dataProcessor.shaders.neightbUpdate.id);
    hasProgram = false;
}

void Solver::deleteBuffers()
{
    dataProcessor.buffers.pointData.deleteBuffer();
    dataProcessor.buffers.mapData.deleteBuffer();
    dataProcessor.buffers.mapSize.deleteBuffer();
    dataProcessor.buffers.ColorMapData.deleteBuffer();
    dataProcessor.buffers.pointPos.deleteBuffer();
    dataProcessor.buffers.mapSizeFloat.deleteBuffer();
    dataProcessor.buffers.neightb.deleteBuffer();
    dataProcessor.buffers.neightbInfo.deleteBuffer();
    dataProcessor.buffers.neightbSort.deleteBuffer();
    dataProcessor.buffers.neightb_size.deleteBuffer();
    hasBuffers = false;
}


//-------------------
bool Solver::setup()
{


    if(!hasProgram)
       createProgram();

    if(!hasBuffers)
        createBuffers();



    return true;
}

void Solver::importDataToBUffers()
{
    bool worms=true;
    //HexData *hexes = new HexData[hexSphereData->Hexes.size()];
    if(ptrInit)
        points = new PointData[pointsNumber];

    int ix=0;
    int iy=50;


    int lineSize=10;//int(sqrt(pointsNumber));
    int stepSizeX=20;//(worldX/1.25f)/lineSize;
    int stepSizeY=20;//(worldY/1.25f)/lineSize;

    int xxs=lineSize*stepSizeX+10;
    int xx=0;

    int wormId=1;

    for(int i = 0; i < pointsNumber; i++){

        PointData tempPoint;
      //  tempPoint.id=i;
        if(worms)
        {
            tempPoint.headTail.x=500000;
            tempPoint.headTail.y=500000;

            if(i>0 and i!=pointsNumber-1)
            {
                tempPoint.prevNext.x=i-1;
                tempPoint.prevNext.y=i+1;

            }

            if(i%lineSize==0)
            {
                iy+=stepSizeY;
            }

            if(iy>worldY-100)
            {
                iy=120;
                xx+=xxs;
                ix+=xx;

            }

            if(i%lineSize==0)
            {
                tempPoint.headTail.x=-11;
                ix=50;

                ix+=xx;
                //iy+=stepSizeY;

                int ii=i-1;
                if(ii>0)
                    points[ii].headTail.y=-22;

                wormId++;
            }


            ix+=stepSizeX;

            tempPoint.coord=glm::vec2(ix,iy);
            tempPoint.velocity=glm::vec2(0,0);
            tempPoint.radius.x=1.0f;//particleRadius_init*(0.5f+(float)rand()/(float)RAND_MAX);

            if(i==pointsNumber-1)
                tempPoint.headTail.y=-22;

            tempPoint.force.x=wormId;


        }
        else
        {
            float randX=((float)rand()/(float)RAND_MAX);
            float randY=((float)rand()/(float)RAND_MAX);
            tempPoint.coord=glm::vec2(worldX/8+randX*worldX/1.25f,worldX/8+randY*worldY/1.25f);
           // tempPoint.coord=glm::vec2(-100.0f+ix,-100+iy);
           // tempPoint.coord=glm::vec2(-500.0f,-500.0f);
            float randVX=2.0f*(-0.5f+(float)rand()/(float)RAND_MAX);
            float randVY=2.0f*(-0.5f+(float)rand()/(float)RAND_MAX);
           // float randVX=0.0;//(-0.5f+(float)rand()/(float)RAND_MAX);
           // float randVY=0.0;//(-0.5f+(float)rand()/(float)RAND_MAX);
            tempPoint.velocity=glm::vec2(randVX*maxVelocity_init,randVY*maxVelocity_init);
            tempPoint.radius.x=1.0f;

             tempPoint.force.x=i+1;
             wormId++;

           // tempPoint.mass=1.0f;
        }


        points[i] = tempPoint;

    }

    dataProcessor.buffers.pointData.setSubData(0, sizeof(PointData)*pointsNumber, points);
 //   delete[] hexes;
     CHECKGLERROR();

    float *colorMap=new float[colorStateMap.size()*4];

    for(int i = 0; i < colorStateMap.size(); i++){

        colorMap[i*4+0]=colorStateMap[i].r;
        colorMap[i*4+1]=colorStateMap[i].g;
        colorMap[i*4+2]=colorStateMap[i].b;
        colorMap[i*4+3]=colorStateMap[i].a;

    }

    dataProcessor.buffers.ColorMapData.setSubData(0, colorStateMap.size()*4*sizeof(float), colorMap);
    delete[] colorMap;

     CHECKGLERROR();


     if(ptrInit)
     pointDist = new VoronoiData[worldElementsAmount];

     for(int i = 0; i < worldElementsAmount; i++)
     {
         VoronoiData tempPoint;
         tempPoint.coord=glm::vec2(0.0f);
         tempPoint.index=glm::vec2(0.0f);
         tempPoint.dist=glm::vec2(0.0f);

         pointDist[i]=tempPoint;

     }

     dataProcessor.buffers.distMapData.setSubData(0, worldElementsAmount*sizeof(VoronoiData), pointDist);

     ptrInit=false;


}

void Solver::update(Sim_parameters& params)
{
    dataProcessor.buffers.pointData.setBindingPoint(PointData_BP);
    dataProcessor.buffers.ColorMapData.setBindingPoint(ColorList_BP);
    dataProcessor.buffers.pointPos.setBindingPoint(PointPos_BP);
    dataProcessor.buffers.mapData.setBindingPoint(MapData_BP);
    dataProcessor.buffers.mapSize.setBindingPoint(MapSize_BP);
    dataProcessor.buffers.mapSizeFloat.setBindingPoint(MapSizeFloat_BP);
    dataProcessor.buffers.neightb.setBindingPoint(Neightb_BP);
    dataProcessor.buffers.neightb_size.setBindingPoint(NeightbSize_BP);
    dataProcessor.buffers.neightbSort.setBindingPoint(NeightbSort_BP);
    dataProcessor.buffers.neightbInfo.setBindingPoint(NeightbInfo_BP);
  //  dataProcessor.buffers.neightbSortSpec.setBindingPoint(NeightSpec_BP);
    dataProcessor.buffers.distMapData.setBindingPoint(DistMapData_BP);


    //for(int i=0; i<dataProcessor.shaders.numberOffsets;i++)


    {
        useProgram(dataProcessor.shaders.mapClear);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
        }
        runComputeShader();
        CHECKGLERROR();
    }

    glUseProgram(0);
    {
        useProgram(dataProcessor.shaders.mapUpdate);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);
        }
        runComputeShader();
        CHECKGLERROR();
    }
    glUseProgram(0);
/*
    {
        auto mapSizedata=new int[mapElementsAmount];
        dataProcessor.buffers.mapSize.getSubData(0, mapElementsAmount*sizeof(int), mapSizedata);

        for(int i=0; i<mapElementsAmount;i++)
        {
            if(i%mapSize.y==0 )
                cout<<endl;
            cout<<mapSizedata[i]<<" ";


        }
        cout<<endl;
        cout<<endl;

        delete [] mapSizedata;
    }
*/

    {
        useProgram(dataProcessor.shaders.neightbUpdate);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

        }
        runComputeShader();
        CHECKGLERROR();
    }
    glUseProgram(0);
/*
    {
        auto mapNeightbdata=new int[pointsNumber*neightbMaxParticles];
        dataProcessor.buffers.neightb.getSubData(0, pointsNumber*neightbMaxParticles*sizeof(int), mapNeightbdata);

        auto mapNeightbSize=new int[pointsNumber];
        dataProcessor.buffers.neightb_size.getSubData(0, pointsNumber*sizeof(int), mapNeightbSize);


        //for(int i=0; i<mapElementsAmount;i++)
        for(int i=0; i<100;i++)
        {
             if(mapNeightbSize[i]>25)
            {
            if(i%mapSize.y==0 )
                cout<<endl;
         //   cout<<i<<"="<<mapNeightbSize[i]<<" ";

                cout<<mapNeightbSize[i]<<" ";
          //  cout<<endl;
             }


        }
        cout<<endl;
        cout<<endl;

        delete [] mapNeightbdata;
        delete [] mapNeightbSize;
    }

*/

    {
        useProgram(dataProcessor.shaders.neightbSort);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

        }
        runComputeShader();
        CHECKGLERROR();
    }
    glUseProgram(0);

    {
        useProgram(dataProcessor.shaders.pointsPhysics);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform1f(glGetUniformLocation(currentProgram.id, "dt"), params.dt);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1f(glGetUniformLocation(currentProgram.id, "maxVelocity"), params.maxVelocity);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "tick"), params.tick);
            glUniform1f(glGetUniformLocation(currentProgram.id, "noiseCoeff"), params.noiseCoeff);
            glUniform1f(glGetUniformLocation(currentProgram.id, "stiffnessCoeff"), params.stiffnessCoeff);
            glUniform1f(glGetUniformLocation(currentProgram.id, "dampingCoeff"), params.dampingCoeff);
        }
        runComputeShader();
        CHECKGLERROR();
    }

   // dataProcessor.buffers.distMapData.setSubData(0, worldElementsAmount*sizeof(glm::vec2), pointDist);


    {
        // glBindTexture(GL_TEXTURE_2D,  dataProcessor.buffers.writeToTexture.id);
        useProgram(dataProcessor.shaders.distMapClear);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

          //  GLuint uniform_location0 = glGetUniformLocation(currentProgram.id, "writeToTexture");
          //  glBindImageTexture(uniform_location0, dataProcessor.buffers.writeToTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            GLuint uniform_location1 = glGetUniformLocation(currentProgram.id, "distinationTexture");
            glBindImageTexture(uniform_location1, dataProcessor.buffers.writeToTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        }
        runComputeShader();
        CHECKGLERROR();

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
       // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
       glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    {
        // glBindTexture(GL_TEXTURE_2D,  dataProcessor.buffers.writeToTexture.id);
        useProgram(dataProcessor.shaders.distMapClear);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

          //  GLuint uniform_location0 = glGetUniformLocation(currentProgram.id, "writeToTexture");
          //  glBindImageTexture(uniform_location0, dataProcessor.buffers.writeToTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            GLuint uniform_location1 = glGetUniformLocation(currentProgram.id, "distinationTexture");
            glBindImageTexture(uniform_location1, dataProcessor.buffers.distMapTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        }
        runComputeShader();
        CHECKGLERROR();

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
       // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }


    glUseProgram(0);
   // dataProcessor.buffers.pointData.getSubData(0, pointsNumber*sizeof(PointData), points);

    {
        useProgram(dataProcessor.shaders.writeToTexture);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

            GLuint uniform_location = glGetUniformLocation(currentProgram.id, "destination");
            glBindImageTexture(uniform_location, dataProcessor.buffers.writeToTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        }
        runComputeShader();
        CHECKGLERROR();

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
    }
    glUseProgram(0);

    //int i=Tick%11;
    for(int i=0;i<2;i++)
    {
        {
            useProgram(dataProcessor.shaders.distMap);{
                glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
                glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
                glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
                glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
                glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
                glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
                glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
                glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);
                glUniform1i(glGetUniformLocation(currentProgram.id, "passNumber"),i);

              //  GLuint uniform_location = glGetUniformLocation(currentProgram.id, "destination");
               // glBindImageTexture(uniform_location, dataProcessor.buffers.distMapTexture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

            }
            runComputeShader();
            CHECKGLERROR();

            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        }

        glUseProgram(0);
   }

    {
        // glBindTexture(GL_TEXTURE_2D,  dataProcessor.buffers.writeToTexture.id);
        useProgram(dataProcessor.shaders.distMapDraw);{
            glUniform1i(glGetUniformLocation(currentProgram.id, "numPoints"),pointsNumber);
            glUniform1f(glGetUniformLocation(currentProgram.id, "radius"), params.radius);
            glUniform2f(glGetUniformLocation(currentProgram.id, "worldSize"), worldSize.x, worldSize.y);
            glUniform2i(glGetUniformLocation(currentProgram.id, "mapSize"), mapSize.x, mapSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "mapDivider"), mapDivider);
            glUniform2f(glGetUniformLocation(currentProgram.id, "mapCellSize"), mapCellSize.x, mapCellSize.y);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxNeightbSize"), neightbMaxParticles);
            glUniform1i(glGetUniformLocation(currentProgram.id, "maxCellSize"), cellMaxParticles);

          //  GLuint uniform_location0 = glGetUniformLocation(currentProgram.id, "writeToTexture");
          //  glBindImageTexture(uniform_location0, dataProcessor.buffers.writeToTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

            GLuint uniform_location1 = glGetUniformLocation(currentProgram.id, "distinationTexture");
            glBindImageTexture(uniform_location1, dataProcessor.buffers.distMapTexture.id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        }
        runComputeShader();
        CHECKGLERROR();

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
       // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
       glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }


/*
    glm::vec3 tempVect;
    dataProcessor.buffers.pointPos.getSubData(0, 3*sizeof(float), glm::value_ptr(tempVect));

    int x=tempVect.x;
*/

}




Solver::~Solver()
{
    deleteBuffers();
    deleteProgram();
}


void Solver::generate_image()
{


}

