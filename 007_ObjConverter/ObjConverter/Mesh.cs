using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;

namespace ObjConverter {
    class Mesh {
        public Mesh(String filename) {
            vertices = new List<Position>( 1000 );
            uvs = new List<UV>( 1000 );
            normals = new List<Normal>( 1000 );
            vertexBuffer = new List<Vertex>( 1000 );
            indexBuffer = new List<int>( 1000 );


            using (StreamReader sr = new StreamReader(filename)) {
                String line = sr.ReadLine( );

                while (line != null){
                    line = line.TrimStart();

                    if(line == "" || line[0] == '#') {
                        line = sr.ReadLine( );
                        continue;
                    }
                    var words = line.Split( new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries );

                    switch (words[0]) {
                        case "v":
                            float x = Convert.ToSingle( words[1] );
                            float y = Convert.ToSingle( words[2] );
                            float z = Convert.ToSingle( words[3] );
                            vertices.Add( new Position{ x = x, y = y, z = z } );
                            break;
                        case "vt":
                            float u = Convert.ToSingle( words[1] );
                            float v = Convert.ToSingle( words[2] );
                            uvs.Add( new UV { u = u, v = v} );
                            break;
                        case "vn":
                            float xn = Convert.ToSingle( words[1] );
                            float yn = Convert.ToSingle( words[2] );
                            float zn = Convert.ToSingle( words[3] );
                            normals.Add( new Normal { x = xn, y = yn, z = zn } );
                            break;
                        case "f":
                            for(int w = 1; w < 4; w++) {
                                var idx = indexBuffer.Count( );
                                var vert = new Vertex( );
                                var verts_string = words[w].Split( '/' );
                                var pos = Convert.ToInt32( verts_string[0] );
                                vert.x = vertices[pos - 1].x;
                                vert.y = vertices[pos - 1].y;
                                vert.z = vertices[pos - 1].z;

                                if (verts_string[1] == "") {
                                    vert.u = 0.0f;
                                    vert.v = 0.0f;
                                } else {
                                    var uv = Convert.ToInt32( verts_string[1] );
                                    vert.u = uvs[uv - 1].u;
                                    vert.v = uvs[uv - 1].v;
                                }

                                var norm = Convert.ToInt32( verts_string[2] );
                                vert.xn = normals[norm - 1].x;
                                vert.yn = normals[norm - 1].y;
                                vert.zn = normals[norm - 1].z;

                                vertexBuffer.Add( vert );
                                indexBuffer.Add( idx );
                            }
                            break;
                    }

                    line = sr.ReadLine( );
                }
            }

            //VertexEntries[] vertex_entries = new VertexEntries()
        }

        public void WriteToFile( string filename ) {
            using (var fs = new FileStream( filename, FileMode.Create )) {
                BinaryWriter bw = new BinaryWriter( fs );

                bw.Write( vertexBuffer.Count( ) );

                for (int i = 0; i < vertexBuffer.Count( ); i++) {
                    bw.Write( vertexBuffer[i].x );
                    bw.Write( vertexBuffer[i].y );
                    bw.Write( vertexBuffer[i].z );
                    bw.Write( vertexBuffer[i].xn );
                    bw.Write( vertexBuffer[i].yn );
                    bw.Write( vertexBuffer[i].zn );
                    bw.Write( vertexBuffer[i].u );
                    bw.Write( vertexBuffer[i].v );
                }
            }
        }

        private struct Position {
            public float x, y, z;
        }

        private struct UV {
            public float u, v;
        }

        private struct Normal {
            public float x, y, z;
        }

        public struct Face {
            public int vertex;
            public int uv;
            public int normal;
        }

        [StructLayout( LayoutKind.Sequential, Pack = 1 )]
        private struct Vertex {
            public float x, y, z, u, v, xn, yn, zn;
            
        }

        /*private struct Vertex {
            public float x, y, z, u, v;

        }*/

        private List<Position> vertices;
        private List<UV> uvs;
        private List<Normal> normals;
        private List<Vertex> vertexBuffer;
        private List<int> indexBuffer;
    }
}
