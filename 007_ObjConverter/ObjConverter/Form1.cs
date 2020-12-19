using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace ObjConverter {
    public partial class Form1 : Form {
        public Form1() {
            InitializeComponent( );
        }

        private void buttonOpenFile_Click( object sender, EventArgs e ) {
            openFileDialog1.ShowDialog( );
        }

        private void openFileDialog1_FileOk( object sender, CancelEventArgs e ) {
            textBoxFileName.Text = openFileDialog1.FileName;
        }

        private void buttonConvert_Click( object sender, EventArgs e ) {
            var file_in = textBoxFileName.Text;
            mesh = new Mesh( file_in );
            var file_out = file_in.Remove( file_in.Count( ) - 4, 4 ) + ".vb";
            mesh.WriteToFile( file_out );
        }

        private Mesh mesh;
    }
}
