#include "hdf5.hpp"

ExtensibleDataSet::ExtensibleDataSet(hid_t file, const char *name, )
{
	hid_t cparms,
	      ds,
	      dsp;
	hsize_t maxdims[2] = {H5S_UNLIMITED, H5S_UNLIMITED};

	this->size[0] = 1;
	this->size[1] = 1;

	// Création du Chunk permettant d'insérer des colonnes ou lignes au fur et à mesure :
	cparms         = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(cparms, 2, this->size);

	// Création de l'espace de travail à allouer :
	dsp            = H5Screate_simple(2, this->size, maxdims);
	// Création du dataset en lui-même :
	ds             = H5Dcreate(file, name, H5T_IEEE_F64LE, dsp, H5P_DEFAULT, cparms, H5P_DEFAULT);

	// Enregistrement des propriétés du Dataset :
	this->dataset   = ds;
	this->file      = file;
	this->offset[0] = 0;
	this->offset[1] = 0;
	this->incremente= false;

	H5Sclose(dsp);
}

ExtensibleDataSet::~ExtensibleDataSet(void)
{
	H5Dclose(this->dataset);
}

void ExtensibleDataSet::SetShape(int a, int b)
{
	this->size[0] = a;
	this->size[1] = b;
}

// ExtensibleDataSet CreateExtensibleDS_integer(hid_t id, const char *sub_grp, hsize_t dims[2]) //const int lig, const int col)
// {
	// ExtensibleDataSet  new = malloc(sizeof( struct _ExtensibleDataSet ));
	// hid_t              cparms;
	// hid_t              ds,
			   // dsp;
	// hsize_t            maxdims[2] = {H5S_UNLIMITED, H5S_UNLIMITED};
	// herr_t             status;

	// // Création du Chunk permettant d'insérer des colonnes ou lignes au fur et à mesure :
	// cparms         = H5Pcreate(H5P_DATASET_CREATE);
	// status         = H5Pset_chunk(cparms, 2, dims);

	// // Création de l'espace de travail à allouer :
	// dsp            = H5Screate_simple(2, dims, maxdims);

	// // Création du dataset en lui-même :
	// ds             = H5Dcreate(id, sub_grp, H5T_STD_I32LE, dsp, H5P_DEFAULT, cparms, H5P_DEFAULT);

	// // Enregistrement des propriétés du Dataset :
	// new->size[0]   = dims[0];
	// new->size[1]   = dims[1];
	// new->dataset   = ds;
	// new->file      = id;
	// new->offset[0] = 0;
	// new->offset[1] = 0;
	// new->incremente= false;

	// H5Sclose(dsp);

	// return new;
	// (void)status;
// }

void ExtensibleDataSet_Extend(ExtensibleDataSet id, const double *data, hsize_t dims[2])
{
	// On étend l'espace alloué au dataset :
	if( id->incremente || id->size[1] < dims[1] )
		id->size[1]     += dims[1];
	else
		id->incremente = true;
	herr_t status    = H5Dextend(id->dataset, id->size);
	// On récupère les infos sur l'espace nouvellement alloué :
	hid_t  filespace = H5Dget_space(id->dataset);
	// On sélectionne la zone où l'on va écrire les nouvelles données :
	status           = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, id->offset, NULL, dims, NULL);
	// On crée le dataspace pour les nouvelles données :
	hid_t  dataspace = H5Screate_simple(2, dims, NULL);

	// On écrit les données :
	status           = H5Dwrite(id->dataset, H5T_NATIVE_DOUBLE, dataspace, filespace, H5P_DEFAULT, data);

	// On incrémente l'offset :
	//id->offset[0]    += dims[0];
	id->offset[1]   += dims[1];

	// On libère la mémoire :
	H5Sclose(filespace);
	H5Sclose(dataspace);
	(void)status;
}

void ExtensibleDataSet_Extend_integer(ExtensibleDataSet id, const double *data, hsize_t dims[2])
{
	// On étend l'espace alloué au dataset :
	if( id->incremente || id->size[1] < dims[1] )
		id->size[1]     += dims[1];
	else
		id->incremente = true;
	herr_t status    = H5Dextend(id->dataset, id->size);
	// On récupère les infos sur l'espace nouvellement alloué :
	hid_t  filespace = H5Dget_space(id->dataset);
	// On sélectionne la zone où l'on va écrire les nouvelles données :
	status           = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, id->offset, NULL, dims, NULL);
	// On crée le dataspace pour les nouvelles données :
	hid_t  dataspace = H5Screate_simple(2, dims, NULL);

	// On écrit les données :
	status           = H5Dwrite(id->dataset, H5T_NATIVE_INT, dataspace, filespace, H5P_DEFAULT, data);

	// On incrémente l'offset :
	//id->offset[0]    += dims[0];
	id->offset[1]   += dims[1];

	// On libère la mémoire :
	H5Sclose(filespace);
	H5Sclose(dataspace);
	(void)status;
}

void ExtensibleDataSet_Close(ExtensibleDataSet id)
{
	H5Dclose(id->dataset);
}

