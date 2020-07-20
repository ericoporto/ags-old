namespace AGS.Editor
{
    partial class SetPlayerStartingPosition
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.numeric_xpos = new System.Windows.Forms.NumericUpDown();
            this.numeric_ypos = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.numeric_xpos)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numeric_ypos)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.button1.Location = new System.Drawing.Point(45, 102);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "OK";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button2.Location = new System.Drawing.Point(146, 102);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 1;
            this.button2.Text = "Cancel";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // numeric_xpos
            // 
            this.numeric_xpos.Location = new System.Drawing.Point(70, 67);
            this.numeric_xpos.Name = "numeric_xpos";
            this.numeric_xpos.Size = new System.Drawing.Size(50, 20);
            this.numeric_xpos.TabIndex = 2;
            // 
            // numeric_ypos
            // 
            this.numeric_ypos.Location = new System.Drawing.Point(172, 67);
            this.numeric_ypos.Name = "numeric_ypos";
            this.numeric_ypos.Size = new System.Drawing.Size(50, 20);
            this.numeric_ypos.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(48, 71);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(17, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "X:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(150, 71);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(17, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Y:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(34, 30);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(204, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Set new player character starting position:";
            // 
            // SetPlayerStartingPosition
            // 
            this.AcceptButton = this.button1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button2;
            this.ClientSize = new System.Drawing.Size(274, 151);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.numeric_ypos);
            this.Controls.Add(this.numeric_xpos);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetPlayerStartingPosition";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Set Starting Position";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SetPlayerStartingPosition_FormClosing);
            this.Shown += new System.EventHandler(this.SetPlayerStartingPosition_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.numeric_xpos)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numeric_ypos)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.NumericUpDown numeric_xpos;
        private System.Windows.Forms.NumericUpDown numeric_ypos;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
    }
}
