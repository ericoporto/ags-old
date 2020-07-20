using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Room = AGS.Types.Room;

namespace AGS.Editor
{
    public partial class SetPlayerStartingPosition : Form
    {
        private Room _room;

        public int PlayerStartingXPos = 0;
        public int PlayerStartingYPos = 0;
        static private int _Clamp(int v, int max, int min)
        {
            return (v < min) ? min : (v > max) ? max : v;
        }
        public SetPlayerStartingPosition(Room room)
        {
            _room = room;
            InitializeComponent();
        }

        private void SetPlayerStartingPosition_FormClosing(object sender, FormClosingEventArgs e)
        {
            PlayerStartingXPos = Convert.ToInt32(numeric_xpos.Value);
            PlayerStartingYPos = Convert.ToInt32(numeric_ypos.Value);
        }

        private void SetPlayerStartingPosition_Shown(object sender, EventArgs e)
        {
            int player_x = Factory.AGSEditor.CurrentGame.PlayerCharacter.StartX;
            int player_y = Factory.AGSEditor.CurrentGame.PlayerCharacter.StartY;
            int room_width = _room.Width;
            int room_height = _room.Height;

            numeric_xpos.Maximum = room_width;
            numeric_xpos.Minimum = 0;
            numeric_xpos.Value = _Clamp(player_x, room_width, 0);

            numeric_ypos.Maximum = room_height;
            numeric_ypos.Minimum = 0;
            numeric_ypos.Value = _Clamp(player_y, room_height, 0);
        }
    }
}
