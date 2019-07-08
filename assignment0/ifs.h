#ifndef _IFS_H_
#define _IFS_H_


class Ifs {

	public:

		Ifs(int num) {
			n = num;
			mats = new Matrix[n];
			prob = new float[n];
		}
		Ifs(const char *filename) {
			FILE *fp = fopen(filename,"r");
			assert(fp);
			
			fscanf(fp, "%d", &n);
			mats = new Matrix[n];
			prob = new float[n];
			
			for (int i = 0; i < n; ++i) {
				fscanf(fp, "%f", &prob[i]);
				mats[i].Read3x3(fp);
			}
			fclose(fp);	
		}
		~Ifs() {
			delete [] mats;
			delete [] prob;
		}
		
		Matrix* RandPickMat();
		
		void Render(int points, int iters, int size, const char *filename);

	private:
		int n;
		Matrix *mats;
		float *prob;
};


#endif
