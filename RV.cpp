#include "vtkBoxWidget.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkAlgorithm.h"
#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageResample.h"
#define VTI_FILETYPE 1
#define MHA_FILETYPE 2;
void PrintUsage();
int main(int argc, char** argv)
{
    // Parse the parameters
    int count=1;
    char* dirname = nullptr;
    double opacityWindow = 4096;
    double opacityLevel = 2048;
    int blendType = 0;
    int clip = 0;
    double reductionFactor = 1.0;
    double frameRate = 10.0;
    char *fileName = nullptr;
    int fileType = 0;
    bool independentComponents = true;

    while (count<argc)
    {   
        if (!strcmp(argv[count],"?")) {
            PrintUsage();
            exit(EXIT_SUCCESS);
        } else if (!strcmp(argv[count],"-DICOM")) {
            size_t size = strlen(argv[count+1]) +1;
            dirname = new char[size];
            snprintf(dirname,size,"%s",argv[count+1]);
            count += 2;
        } else if (!strcmp(argv[count+1],"-VTI")){
            size_t size = strlen(argv[count+1]) +1;
            fileName = new char[size];
            fileType = VTI_FILETYPE;
            snprintf(fileName,size,"%s",argv[count+1]);
            count += 2;
        } else if (!strcmp(argv[count+1],"-MHA")){
            size_t size = strlen(argv[count+1]) +1;
            fileName = new char[size];
            fileType = MHA_FILETYPE;
            snprintf(fileName,size,"%s",argv[count+1]);
            count += 2;
        } else if (!strcmp(argv[count],"-Clip")){
            clip = 1;
            count++;
        } else if (!strcmp(argv[count],"-MIP")){
            opacityWindow = atof(argv[count+1]);
            opacityLevel = atof(argv[count + 2]);
            blendType = 0;
            count +=3;
        } else if ( !strcmp( argv[count], "-CompositeRamp" ) )
    {
      opacityWindow = atof( argv[count+1] );
      opacityLevel  = atof( argv[count+2] );
      blendType = 1;
      count += 3;
    }
    else if ( !strcmp( argv[count], "-CompositeShadeRamp" ) )
    {
      opacityWindow = atof( argv[count+1] );
      opacityLevel  = atof( argv[count+2] );
      blendType = 2;
      count += 3;
    }
    else if ( !strcmp( argv[count], "-CT_Skin" ) )
    {
      blendType = 3;
      count += 1;
    }
    else if ( !strcmp( argv[count], "-CT_Bone" ) )
    {
      blendType = 4;
      count += 1;
    }
    else if ( !strcmp( argv[count], "-CT_Muscle" ) )
    {
      blendType = 5;
      count += 1;
    }
    else if ( !strcmp( argv[count], "-RGB_Composite" ) )
    {
      blendType = 6;
      count += 1;
    }
    else if ( !strcmp( argv[count], "-FrameRate") )
    {
      frameRate = atof( argv[count+1] );
      if ( frameRate < 0.01 || frameRate > 60.0 )
      {
        cout << "Invalid frame rate - use a number between 0.01 and 60.0" << endl;
        cout << "Using default frame rate of 10 frames per second." << endl;
        frameRate = 10.0;
      }
      count += 2;
    }
    else if ( !strcmp( argv[count], "-ReductionFactor") )
    {
      reductionFactor = atof( argv[count+1] );
      if ( reductionFactor <= 0.0 || reductionFactor >= 1.0 )
      {
        cout << "Invalid reduction factor - use a number between 0 and 1 (exclusive)" << endl;
        cout << "Using the default of no reduction." << endl;
        reductionFactor = 1.0;
      }
      count += 2;
    }
     else if ( !strcmp( argv[count], "-DependentComponents") )
     {
      independentComponents=false;
      count += 1;
     }
    else
    {
      cout << "Unrecognized option: " << argv[count] << endl;
      cout << endl;
      PrintUsage();
      exit(EXIT_FAILURE);
    }
        
    }
    
    if ( !dirname && !fileName)
  {
    cout << "Error: you must specify a directory of DICOM data or a .vti file or a .mha!" << endl;
    cout << endl;
    PrintUsage();
    exit(EXIT_FAILURE);
  }

    //Create teh renderer
    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);

    //connectir it all.
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);
    iren->SetDesiredUpdateRate(frameRate/(1+clip));
    iren->GetInteractorStyle()->SetDefaultRenderer(renderer);

    //Read data
    vtkAlgorithm *reader = nullptr;
    vtkImageData* input = nullptr;
    if (dirname) {
        vtkDICOMImageReader* dicomReader = vtkDICOMImageReader::New();
        dicomReader->SetDirectoryName(dirname);
        dicomReader->Update();
        input = dicomReader->GetOutput();
        reader  = dicomReader;
    }

     // Verify that we actually have a volume
    int dim[3];
    input->GetDimensions(dim);
    if ( dim[0] < 2 ||
       dim[1] < 2 ||
       dim[2] < 2 )
    {
     cout << "Error loading data!" << endl;
     exit(EXIT_FAILURE);
    }

    
    //cout<<dim[2]<<endl;
    vtkImageResample *resample = vtkImageResample::New();
    if (reductionFactor , 1.0) {
        resample->SetInputConnection(reader->GetOutputPort());
        resample->SetAxisMagnificationFactor(0,reductionFactor);
        resample->SetAxisMagnificationFactor(1,reductionFactor);
        resample->SetAxisMagnificationFactor(2,reductionFactor);
    }

    //create Volume and mapper

    return 0;
}



void PrintUsage()
{
    cout<<"Usage: "<<endl;
    cout<<"R1 <Option>"<<endl;
    cout<<"where option may include: \n";
    cout<<" -DICOM <directory>"<<endl;
    cout<<" -VTI <filename>"<<endl;
    cout<<" -MHA <filename>"<<endl;
    cout<<" -DependenceComponents"<<endl;
    cout<<" -Clip"<<endl;
    cout<<" -MIP <window> <level>"<<endl;
    cout<<" -CompositeRamp <window> <level>"<<endl;
    cout<<" -CompositeShadeRamp <window> <level>"<<endl;
    cout<<" -CT_Skin"<<endl;
    cout<<" -CT_Bone"<<endl;
    cout<<" -CT_Muscle"<<endl;
    cout<<" -FrameRate <rate>"<<endl;
    cout<<" -DataReduction <factor>"<<endl;
    cout<<"You must you either -DICOM option to specify the directory where "<<endl;
    cout<<"the data is located or the -VTI or -MHA option to specify the path of a .VTI file"<<endl;
    cout<<"By default, the programe assumes that the file has independent components"<<endl;
    cout<<"use -DependenceComponents to specify that the file has dependent components."<<endl;
    cout<<"Use the -Clip option to display a cude widget for clipping the colume"<<endl;
    cout<<"Use the -FrameRate option with a desred frame rate (in frames per second)"<<endl;
    cout<<"which will control the interactive rendering rate"<<endl;
    cout<<"Use the -DateReduction option with reduction feactor in [0,1] to reduve data before rendering"<<endl;
    cout<<"Use one og the remaning option to specify the blend function and transfer function."<<endl;
    cout<<" The -MIP option utinizes a maxium intensity projection method, while other utilizes compositing"<<endl;
    cout<<"The -CompisiteRamp option s unshaded compositing, while the other"<<endl;
    cout<<"compositing option employ shading."<<endl;
    cout<<"Note: MIP, CompositeRamp, CompositeShadeRamp, CT_Slin, CT_Bone"<<endl;
    cout<<" and CT_Muscle are appropriate for DICOM data. MIP, CompositeRamp, "<<endl;
    cout<<"and RGB_Composite are appropriate for RGB data."<<endl;
    cout<<"Example: R1 -DICOM CTNeck -MIP 4096 1024"<<endl;
}